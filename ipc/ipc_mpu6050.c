/*
 * ipc_mpu6050.c — MPU6050 IPC 实现
 *
 * 通过 D-Bus 信号 Mpu6050Data / Mpu6050Behavior 接收数据，
 * 回调通知上层进行姿态解算和驾驶行为统计。
 */
#include "ipc_mpu6050.h"
#include "lvgl_dbus_protocol.h"
#include "ipc_base.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static DBusConnection        *g_conn = NULL;
static Mpu6050_Data_t         g_data;
static pthread_mutex_t        g_data_mutex = PTHREAD_MUTEX_INITIALIZER;
static ipc_mpu6050_data_cb_t  g_user_cb = NULL;
static void                  *g_user_data = NULL;
static int                    g_service_connected = 0;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    /* 监听服务 NameOwnerChanged 实现服务发现与重连 */
    if (dbus_message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged")) {
        const char *nm = NULL, *old_owner = NULL, *new_owner = NULL;
        if (dbus_message_get_args(msg, NULL,
                DBUS_TYPE_STRING, &nm,
                DBUS_TYPE_STRING, &old_owner,
                DBUS_TYPE_STRING, &new_owner,
                DBUS_TYPE_INVALID)) {
            if (nm && strcmp(nm, MPU6050_SERVICE_NAME) == 0) {
                g_service_connected = (new_owner && new_owner[0] != '\0') ? 1 : 0;
                printf("[ipc_mpu6050] service %s\n",
                       g_service_connected ? "appeared" : "disappeared");
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    if (!dbus_message_is_signal(msg, MPU6050_IFACE_NAME, MPU6050_SIGNAL_DATA))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    DBusError err;
    dbus_error_init(&err);

    double ax_mss, ay_mss, az_mss;
    double gx_dps, gy_dps, gz_dps;
    double pitch, roll;
    double acc_mag, gyro_mag;
    int32_t behavior;

    if (!dbus_message_get_args(msg, &err,
            DBUS_TYPE_DOUBLE, &ax_mss,
            DBUS_TYPE_DOUBLE, &ay_mss,
            DBUS_TYPE_DOUBLE, &az_mss,
            DBUS_TYPE_DOUBLE, &gx_dps,
            DBUS_TYPE_DOUBLE, &gy_dps,
            DBUS_TYPE_DOUBLE, &gz_dps,
            DBUS_TYPE_DOUBLE, &pitch,
            DBUS_TYPE_DOUBLE, &roll,
            DBUS_TYPE_DOUBLE, &acc_mag,
            DBUS_TYPE_DOUBLE, &gyro_mag,
            DBUS_TYPE_INT32,  &behavior,
            DBUS_TYPE_INVALID)) {
        fprintf(stderr, "[ipc_mpu6050] get_args failed: %s\n", err.message);
        dbus_error_free(&err);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    pthread_mutex_lock(&g_data_mutex);
    g_data.ax_mss   = (float)ax_mss;
    g_data.ay_mss   = (float)ay_mss;
    g_data.az_mss   = (float)az_mss;
    g_data.gx_dps   = (float)gx_dps;
    g_data.gy_dps   = (float)gy_dps;
    g_data.gz_dps   = (float)gz_dps;
    g_data.pitch_deg = (float)pitch;
    g_data.roll_deg  = (float)roll;
    g_data.acc_mag  = (float)acc_mag;
    g_data.gyro_mag = (float)gyro_mag;
    g_data.behavior = (mpu6050_behavior_t)behavior;
    pthread_mutex_unlock(&g_data_mutex);

    if (g_user_cb)
        g_user_cb(&g_data, g_user_data);

    return DBUS_HANDLER_RESULT_HANDLED;
}

int ipc_mpu6050_init(const char *bus_address,
                     ipc_mpu6050_data_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = ipc_base_connect(bus_address);
    if (!g_conn) return -1;

    const char *rule = "type='signal',interface='" MPU6050_IFACE_NAME "',member='" MPU6050_SIGNAL_DATA "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_mpu6050] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_mpu6050] add_filter failed\n");
        dbus_connection_unref(g_conn);
        g_conn = NULL;
        return -1;
    }

    /* 监听 NameOwnerChanged 实现服务发现与重连 */
    ipc_base_watch_service(g_conn, MPU6050_SERVICE_NAME);

    /* 等待 mpu6050_service 上线 */
    if (ipc_base_wait_for_service(g_conn, MPU6050_SERVICE_NAME, 25) == 0)
        g_service_connected = 1;
    else
        fprintf(stderr, "[ipc_mpu6050] WARNING: mpu6050_service not found after 500ms\n");

    g_user_cb   = cb;
    g_user_data = user_data;

    printf("[ipc_mpu6050] initialized, connected to %s\n", bus_address);
    return 0;
}

int ipc_mpu6050_get_data(Mpu6050_Data_t *data)
{
    if (!data) return -1;
    pthread_mutex_lock(&g_data_mutex);
    *data = g_data;
    pthread_mutex_unlock(&g_data_mutex);
    return 0;
}

void ipc_mpu6050_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_mpu6050_deinit(void)
{
    ipc_base_disconnect(&g_conn, filter_cb);
    pthread_mutex_destroy(&g_data_mutex);
    printf("[ipc_mpu6050] deinitialized\n");
}