#include "ipc_can.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CAN_SERVICE_NAME  "com.lvgl.demo.CAN"
#define CAN_OBJECT_PATH   "/com/lvgl/demo/CAN"
#define CAN_INTERFACE     "com.lvgl.demo.CAN"

static DBusConnection *g_conn = NULL;
static Encoder_Data_t   g_encoder;
static pthread_mutex_t  g_enc_mutex = PTHREAD_MUTEX_INITIALIZER;
static ipc_can_encoder_cb_t g_user_cb = NULL;
static void            *g_user_data = NULL;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (!dbus_message_is_signal(msg, CAN_INTERFACE, "EncoderUpdated"))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    DBusError err;
    dbus_error_init(&err);

    int32_t count;
    double  speed, angle;

    if (!dbus_message_get_args(msg, &err,
            DBUS_TYPE_INT32,  &count,
            DBUS_TYPE_DOUBLE, &speed,
            DBUS_TYPE_DOUBLE, &angle,
            DBUS_TYPE_INVALID)) {
        fprintf(stderr, "[ipc_can] get_args failed: %s\n", err.message);
        dbus_error_free(&err);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    pthread_mutex_lock(&g_enc_mutex);
    g_encoder.count     = (int16_t)count;
    g_encoder.speed_rpm = (float)speed;
    g_encoder.angle     = (float)angle;
    g_encoder.dir       = 0;
    pthread_mutex_unlock(&g_enc_mutex);

    if (g_user_cb)
        g_user_cb(&g_encoder, g_user_data);

    return DBUS_HANDLER_RESULT_HANDLED;
}

int ipc_can_init(const char *bus_address,
                 ipc_can_encoder_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_can] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_can] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" CAN_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_can] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_can] add_filter failed\n");
        return -1;
    }

    g_user_cb   = cb;
    g_user_data = user_data;

    printf("[ipc_can] initialized, connected to %s\n", bus_address);
    return 0;
}

int ipc_can_get_encoder(Encoder_Data_t *enc)
{
    if (!enc) return -1;
    pthread_mutex_lock(&g_enc_mutex);
    *enc = g_encoder;
    pthread_mutex_unlock(&g_enc_mutex);
    return 0;
}

int ipc_can_send_frame(uint32_t id, const uint8_t *data, uint8_t len)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        CAN_SERVICE_NAME, CAN_OBJECT_PATH, CAN_INTERFACE, "SendFrame");

    if (!msg) return -1;

    DBusMessageIter iter, array;
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &id);

    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
        DBUS_TYPE_BYTE_AS_STRING, &array);

    for (uint8_t i = 0; i < len; i++)
        dbus_message_iter_append_basic(&array, DBUS_TYPE_BYTE, &data[i]);

    dbus_message_iter_close_container(&iter, &array);

    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    return 0;
}

void ipc_can_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_can_deinit(void)
{
    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    pthread_mutex_destroy(&g_enc_mutex);
    printf("[ipc_can] deinitialized\n");
}