#include "ipc_uwb.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define UWB_SERVICE_NAME  "com.lvgl.demo.UWB"
#define UWB_OBJECT_PATH   "/com/lvgl/demo/UWB"
#define UWB_INTERFACE     "com.lvgl.demo.UWB"

static DBusConnection    *g_conn = NULL;
static Uwb_Data_t         g_data;
static pthread_mutex_t    g_data_mutex = PTHREAD_MUTEX_INITIALIZER;
static ipc_uwb_data_cb_t  g_user_cb = NULL;
static void              *g_user_data = NULL;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (!dbus_message_is_signal(msg, UWB_INTERFACE, "UwbDataUpdated"))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

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

int ipc_uwb_init(const char *bus_address,
                 ipc_uwb_data_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_uwb] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_uwb] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" UWB_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_uwb] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_uwb] add_filter failed\n");
        return -1;
    }

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

void ipc_uwb_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_uwb_deinit(void)
{
    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    pthread_mutex_destroy(&g_data_mutex);
    printf("[ipc_uwb] deinitialized\n");
}