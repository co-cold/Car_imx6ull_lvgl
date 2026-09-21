/*
 * ipc_uwb.c — UWB IPC 实现
 *
 * 通过 D-Bus 信号 UwbData / StatusChanged 接收 uwb_service 数据，
 * 回调通知上层（sensor_fusion）进行融合处理。
 */
#include "ipc_uwb.h"
#include "lvgl_dbus_protocol.h"
#include "ipc_base.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static DBusConnection       *g_conn = NULL;
static Uwb_Data_t            g_data;
static pthread_mutex_t       g_data_mutex = PTHREAD_MUTEX_INITIALIZER;
static ipc_uwb_data_cb_t     g_user_cb = NULL;
static void                 *g_user_data = NULL;
static ipc_uwb_status_cb_t   g_status_cb = NULL;
static void                 *g_status_user_data = NULL;
static int                   g_connected = 0;
static int                   g_service_connected = 0;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, UWB_IFACE_NAME, UWB_SIGNAL_DATA)) {
        DBusError err;
        dbus_error_init(&err);

        uint32_t dist_mm, dist_filt_mm;
        double   change_rate;
        int32_t  rssi;
        uint32_t raw_ts, timestamp_ms;
        uint32_t seq;

        if (!dbus_message_get_args(msg, &err,
                DBUS_TYPE_UINT32, &dist_mm,
                DBUS_TYPE_UINT32, &dist_filt_mm,
                DBUS_TYPE_DOUBLE, &change_rate,
                DBUS_TYPE_INT32,  &rssi,
                DBUS_TYPE_UINT32, &raw_ts,
                DBUS_TYPE_UINT32, &timestamp_ms,
                DBUS_TYPE_UINT32, &seq,
                DBUS_TYPE_INVALID)) {
            fprintf(stderr, "[ipc_uwb] get_args failed: %s\n", err.message);
            dbus_error_free(&err);
            return DBUS_HANDLER_RESULT_HANDLED;
        }

        pthread_mutex_lock(&g_data_mutex);
        g_data.distance_mm          = dist_mm;
        g_data.distance_filtered_mm = dist_filt_mm;
        g_data.distance_change_rate = (float)change_rate;
        g_data.rssi                 = rssi;
        g_data.raw_timestamp        = raw_ts;
        g_data.timestamp_ms         = timestamp_ms;
        g_data.seq                  = (uint16_t)seq;
        pthread_mutex_unlock(&g_data_mutex);

        if (g_user_cb)
            g_user_cb(&g_data, g_user_data);

        return DBUS_HANDLER_RESULT_HANDLED;
    }

    if (dbus_message_is_signal(msg, UWB_IFACE_NAME, UWB_SIGNAL_STATUS)) {
        const char *status = NULL;
        DBusError err;
        dbus_error_init(&err);

        if (dbus_message_get_args(msg, &err,
                DBUS_TYPE_STRING, &status,
                DBUS_TYPE_INVALID)) {
            int was_connected = g_connected;
            g_connected = (status && strcmp(status, "connected") == 0) ? 1 : 0;

            if (g_connected != was_connected && g_status_cb)
                g_status_cb(g_connected, g_status_user_data);

            printf("[ipc_uwb] status changed: %s (connected=%d)\n",
                   status ? status : "null", g_connected);
        }
        dbus_error_free(&err);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    /* 监听服务 NameOwnerChanged 实现服务发现与重连 */
    if (dbus_message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged")) {
        const char *nm = NULL, *old_owner = NULL, *new_owner = NULL;
        if (dbus_message_get_args(msg, NULL,
                DBUS_TYPE_STRING, &nm,
                DBUS_TYPE_STRING, &old_owner,
                DBUS_TYPE_STRING, &new_owner,
                DBUS_TYPE_INVALID)) {
            if (nm && strcmp(nm, UWB_SERVICE_NAME) == 0) {
                g_service_connected = (new_owner && new_owner[0] != '\0') ? 1 : 0;
                printf("[ipc_uwb] service %s\n",
                       g_service_connected ? "appeared" : "disappeared");
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int ipc_uwb_init(const char *bus_address,
                 ipc_uwb_data_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = ipc_base_connect(bus_address);
    if (!g_conn) return -1;

    const char *rules[] = {
        "type='signal',interface='" UWB_IFACE_NAME "',member='" UWB_SIGNAL_DATA "'",
        "type='signal',interface='" UWB_IFACE_NAME "',member='" UWB_SIGNAL_STATUS "'"
    };
    for (int i = 0; i < 2; ++i) {
        dbus_bus_add_match(g_conn, rules[i], &err);
        if (dbus_error_is_set(&err)) {
            fprintf(stderr, "[ipc_uwb] add_match[%d] failed: %s\n", i, err.message);
            dbus_error_free(&err);
        }
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_uwb] add_filter failed\n");
        dbus_connection_unref(g_conn);
        g_conn = NULL;
        return -1;
    }

    /* 监听 NameOwnerChanged 实现服务发现与重连 */
    ipc_base_watch_service(g_conn, UWB_SERVICE_NAME);

    /* 等待 uwb_service 上线 */
    if (ipc_base_wait_for_service(g_conn, UWB_SERVICE_NAME, 25) == 0)
        g_service_connected = 1;
    else
        fprintf(stderr, "[ipc_uwb] WARNING: uwb_service not found after 500ms\n");

    g_user_cb   = cb;
    g_user_data = user_data;

    printf("[ipc_uwb] initialized, connected to %s\n", bus_address);
    return 0;
}

int ipc_uwb_get_data(Uwb_Data_t *data)
{
    if (!data) return -1;
    pthread_mutex_lock(&g_data_mutex);
    *data = g_data;
    pthread_mutex_unlock(&g_data_mutex);
    return 0;
}

int ipc_uwb_is_connected(void)
{
    return g_connected;
}

void ipc_uwb_set_status_callback(ipc_uwb_status_cb_t cb, void *user_data)
{
    g_status_cb        = cb;
    g_status_user_data = user_data;
}

void ipc_uwb_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_uwb_deinit(void)
{
    ipc_base_disconnect(&g_conn, filter_cb);
    pthread_mutex_destroy(&g_data_mutex);
    printf("[ipc_uwb] deinitialized\n");
}