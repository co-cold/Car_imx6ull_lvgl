/*
 * mpu6050_service.c — MPU6050 六轴传感器 D-Bus 服务
 *
 * 功能：通过 IIO sysfs 读取加速度/角速度，互补滤波计算姿态角，
 *       实时检测驾驶行为（急刹/急加速/转向）。
 *       通过 D-Bus 信号 Mpu6050Data / Mpu6050Behavior 对外广播。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "mpu6050_drv.h"
#include "ipc/lvgl_dbus_protocol.h"
#define DEFAULT_DEVICE "/dev/I2C1_mpu6050"

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static mpu6050_drv_t g_mpu6050_drv;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_mpu6050_signal(DBusConnection *conn, const Mpu6050_Data_t *data)
{
    DBusMessage *msg = dbus_message_new_signal(
        MPU6050_OBJECT_PATH, MPU6050_IFACE_NAME, MPU6050_SIGNAL_DATA);
    if (!msg) return;

    double ax_mss  = data->ax_mss;
    double ay_mss  = data->ay_mss;
    double az_mss  = data->az_mss;
    double gx_dps  = data->gx_dps;
    double gy_dps  = data->gy_dps;
    double gz_dps  = data->gz_dps;
    double pitch   = data->pitch_deg;
    double roll    = data->roll_deg;
    double acc_mag = data->acc_mag;
    double gyro_mag = data->gyro_mag;
    int32_t behavior = (int32_t)data->behavior;

    DBusMessageIter iter;
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &ax_mss);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &ay_mss);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &az_mss);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &gx_dps);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &gy_dps);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &gz_dps);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &pitch);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &roll);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &acc_mag);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &gyro_mag);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,  &behavior);

    dbus_connection_send(conn, msg, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(msg);
}

static DBusHandlerResult method_handler(DBusConnection *conn,
    DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusObjectPathVTable g_vtable = {
    .message_function = method_handler,
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    setlinebuf(stdout);
    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    char   detected[64] = {0};
    const char *device   = NULL;

    if (argc >= 2) {
        device = argv[1];
    } else {
        if (mpu6050_drv_autodetect(detected, sizeof(detected)) == 0) {
            device = detected;
        } else {
            fprintf(stderr, "[mpu6050_service] auto-detect failed, trying default\n");
            device = DEFAULT_DEVICE;
        }
    }

    if (mpu6050_drv_init(&g_mpu6050_drv, device) != 0) {
        fprintf(stderr, "[mpu6050_service] MPU6050 driver init failed (%s)\n", device);
        return 1;
    }

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(PROTO_BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[mpu6050_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[mpu6050_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, MPU6050_SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[mpu6050_service] request_name failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[mpu6050_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, MPU6050_OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[mpu6050_service] register object failed\n");
        return 1;
    }

    printf("[mpu6050_service] D-Bus service registered: %s\n", MPU6050_SERVICE_NAME);

    Mpu6050_Data_t last_data;
    int            last_valid = 0;
    /* uint32_t       last_loop  = 0; */

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

        Mpu6050_Data_t data;
        if (mpu6050_drv_get_data(&g_mpu6050_drv, &data) == 0) {
            if (!last_valid ||
                data.pitch_deg != last_data.pitch_deg ||
                data.roll_deg  != last_data.roll_deg ||
                data.acc_mag   != last_data.acc_mag ||
                data.behavior  != last_data.behavior) {

                emit_mpu6050_signal(g_conn, &data);
                last_data  = data;
                last_valid = 1;
            }
        }

        /* 每 ~5 秒打印一次状态 —— 调试时可打开 */
        /*
        last_loop++;
        if (last_loop % 250 == 0) {
            printf("[mpu6050_service] rx=%u err=%u ax=%.2f ay=%.2f az=%.2f | pitch=%.1f roll=%.1f | beh=%d\n",
                g_mpu6050_drv.rx_count, g_mpu6050_drv.err_count,
                data.ax_mss, data.ay_mss, data.az_mss,
                data.pitch_deg, data.roll_deg, data.behavior);
        }
        */

        usleep(50000);
    }

    printf("[mpu6050_service] shutting down...\n");
    mpu6050_drv_deinit(&g_mpu6050_drv);
    dbus_connection_unref(g_conn);
    return 0;
}