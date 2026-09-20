/*
 * ipc_obd2.c — OBD-II IPC 实现
 *
 * 通过 D-Bus 信号 EncoderSignal 接收 obd2_service 数据，
 * 回调通知上层进行传感器融合。
 */
#include "ipc_obd2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OBD2_SERVICE_NAME  "com.lvgl.demo.CAN"
#define OBD2_OBJECT_PATH   "/com/lvgl/demo/CAN"
#define OBD2_INTERFACE     "com.lvgl.demo.CAN"

static DBusConnection    *g_conn = NULL;
static Encoder_Data_t     g_data;
static pthread_mutex_t    g_data_mutex = PTHREAD_MUTEX_INITIALIZER;
static ipc_obd2_data_cb_t g_user_cb = NULL;
static void              *g_user_data = NULL;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (!dbus_message_is_signal(msg, OBD2_INTERFACE, "EncoderUpdated"))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    DBusError err;
    dbus_error_init(&err);

    int32_t count;
    double  speed;
    double  angle;

    if (!dbus_message_get_args(msg, &err,
            DBUS_TYPE_INT32,  &count,
            DBUS_TYPE_DOUBLE, &speed,
            DBUS_TYPE_DOUBLE, &angle,
            DBUS_TYPE_INVALID)) {
        fprintf(stderr, "[ipc_obd2] get_args failed: %s\n", err.message);
        dbus_error_free(&err);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    pthread_mutex_lock(&g_data_mutex);
    g_data.count     = (int16_t)count;
    g_data.speed_rpm = (float)speed;
    g_data.angle     = (float)angle;
    g_data.dir       = 0;
    pthread_mutex_unlock(&g_data_mutex);

    if (g_user_cb)
        g_user_cb(&g_data, g_user_data);

    return DBUS_HANDLER_RESULT_HANDLED;
}

int ipc_obd2_init(const char *bus_address,
                  ipc_obd2_data_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_obd2] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_obd2] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" OBD2_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_obd2] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_obd2] add_filter failed\n");
        return -1;
    }

    g_user_cb   = cb;
    g_user_data = user_data;

    printf("[ipc_obd2] initialized, connected to %s\n", bus_address);
    return 0;
}

int ipc_obd2_get_data(Encoder_Data_t *data)
{
    if (!data) return -1;
    pthread_mutex_lock(&g_data_mutex);
    *data = g_data;
    pthread_mutex_unlock(&g_data_mutex);
    return 0;
}

void ipc_obd2_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_obd2_deinit(void)
{
    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    pthread_mutex_destroy(&g_data_mutex);
    printf("[ipc_obd2] deinitialized\n");
}