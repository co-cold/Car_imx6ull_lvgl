/*
 * uwb_service.c — UWB 定位 D-Bus 服务
 *
 * 功能：串口读取 UWB 基站数据，通过 D-Bus 信号 UwbData 对外广播。
 *       自动检测 ttyUSB/ttyACM 设备，支持运行时断线重连（热插拔）。
 *       同时发送 StatusChanged 信号通知设备在线/离线状态。
 *
 * 信号：
 *   com.lvgl.demo.UWB.UwbData       — 距离/RSSI/序列号
 *   com.lvgl.demo.UWB.StatusChanged — "connected" / "disconnected"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "uwb_driver.h"
#include "ipc/lvgl_dbus_protocol.h"
#define DEFAULT_BAUD   460800

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static uwb_drv_t g_uwb_drv;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_uwb_signal(DBusConnection *conn, const Uwb_Data_t *data)
{
    DBusMessage *msg = dbus_message_new_signal(
        UWB_OBJECT_PATH, UWB_IFACE_NAME, UWB_SIGNAL_DATA);
    if (!msg) return;

    uint32_t dist_mm      = data->distance_mm;
    uint32_t dist_filt_mm = data->distance_filtered_mm;
    double   change_rate  = data->distance_change_rate;
    int32_t  rssi         = data->rssi;
    uint32_t raw_ts       = data->raw_timestamp;
    uint32_t timestamp_ms = data->timestamp_ms;
    uint32_t seq          = data->seq;

    DBusMessageIter iter;
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &dist_mm);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &dist_filt_mm);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &change_rate);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,  &rssi);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &raw_ts);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &timestamp_ms);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &seq);

    dbus_connection_send(conn, msg, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(msg);
}

static void emit_status_signal(DBusConnection *conn, const char *status)
{
    DBusMessage *msg = dbus_message_new_signal(
        UWB_OBJECT_PATH, UWB_IFACE_NAME, UWB_SIGNAL_STATUS);
    if (!msg) return;
    dbus_message_append_args(msg, DBUS_TYPE_STRING, &status, DBUS_TYPE_INVALID);
    dbus_connection_send(conn, msg, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(msg);
}

static void signal_handler(DBusConnection *conn)
{
    dbus_connection_read_write_dispatch(conn, 0);
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

    int   baudrate = DEFAULT_BAUD;
    if (argc >= 3) baudrate = atoi(argv[2]);

    char detected[64]  = {0};
    bool device_alive  = false;

    /* 确定设备路径 */
    if (argc >= 2) {
        strncpy(detected, argv[1], sizeof(detected) - 1);
    } else {
        printf("[uwb_service] no device specified, auto-detecting...\n");
        /* 不在这里退出 — 等会儿重试 */
    }

    /* D-Bus 连接 — 无论有没有 UWB 设备都要成功 */
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(PROTO_BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[uwb_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[uwb_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, UWB_SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[uwb_service] request_name failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[uwb_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, UWB_OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[uwb_service] register object failed\n");
        return 1;
    }

    printf("[uwb_service] D-Bus service registered: %s\n", UWB_SERVICE_NAME);

    /* 首次尝试连接设备 */
    if (detected[0] == '\0') {
        if (uwb_drv_autodetect(detected, sizeof(detected), baudrate) == 0) {
            device_alive = true;
        }
    } else {
        device_alive = true;
    }

    if (device_alive && uwb_drv_init(&g_uwb_drv, detected, baudrate) == 0) {
        emit_status_signal(g_conn, "connected");
    } else {
        device_alive = false;
        emit_status_signal(g_conn, "disconnected");
    }

    Uwb_Data_t last_data;
    int        last_valid = 0;
    int        reconnect_delay = 0;

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

        uwb_state_t state = uwb_drv_get_state(&g_uwb_drv);

        if (state == UWB_STATE_RUNNING) {
            /* 正常读取数据 */
            Uwb_Data_t data;
            if (uwb_drv_get_data(&g_uwb_drv, &data) == 0) {
                if (!last_valid ||
                    data.distance_filtered_mm != last_data.distance_filtered_mm ||
                    data.seq != last_data.seq) {
                    emit_uwb_signal(g_conn, &data);
                    last_data  = data;
                    last_valid = 1;
                }
            }
            reconnect_delay = 0;  /* 连上了，重置延时 */
        } else {
            /* 设备掉线 — 尝试重连 */
            if (reconnect_delay == 0) {
                emit_status_signal(g_conn, "disconnected");
                fprintf(stderr, "[uwb_service] device lost, retrying...\n");
            }
            reconnect_delay++;

            /* 每 2 秒尝试一次重连 */
            if (reconnect_delay >= 40) {  /* 40 * 50ms = 2s */
                if (detected[0] != '\0' &&
                    uwb_drv_reconnect(&g_uwb_drv, detected, baudrate) == 0) {
                    emit_status_signal(g_conn, "connected");
                    printf("[uwb_service] reconnected to %s\n", detected);
                    reconnect_delay = 0;
                    last_valid = 0;
                } else {
                    /* 指定设备重连失败，尝试自动检测 */
                    if (uwb_drv_autodetect(detected, sizeof(detected), baudrate) == 0 &&
                        uwb_drv_reconnect(&g_uwb_drv, detected, baudrate) == 0) {
                        emit_status_signal(g_conn, "connected");
                        printf("[uwb_service] reconnected to %s\n", detected);
                        reconnect_delay = 0;
                        last_valid = 0;
                    } else {
                        reconnect_delay = 39;  /* 继续重试 */
                    }
                }
            }
        }

        usleep(50000);
    }

    printf("[uwb_service] shutting down...\n");
    uwb_drv_deinit(&g_uwb_drv);
    dbus_connection_unref(g_conn);
    return 0;
}