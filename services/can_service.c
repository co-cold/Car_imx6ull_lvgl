#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "can_drv.h"

#define SERVICE_NAME   "com.lvgl.demo.CAN"
#define OBJECT_PATH    "/com/lvgl/demo/CAN"
#define INTERFACE_NAME "com.lvgl.demo.CAN"
#define BUS_ADDRESS    "unix:path=/tmp/lvgl-dbus-session"

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static can_drv_t g_can_drv;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_encoder_signal(DBusConnection *conn, const Encoder_Data_t *enc)
{
    DBusMessage *msg = dbus_message_new_signal(
        OBJECT_PATH, INTERFACE_NAME, "EncoderUpdated");
    if (!msg) return;

    int32_t count = enc->count;
    double  speed = enc->speed_rpm;
    double  angle = enc->angle;

    DBusMessageIter iter;
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,  &count);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &speed);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &angle);

    dbus_connection_send(conn, msg, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(msg);
}

static DBusHandlerResult method_handler(DBusConnection *conn,
    DBusMessage *msg, void *data)
{
    (void)data;

    if (dbus_message_is_method_call(msg, INTERFACE_NAME, "SendFrame")) {
        DBusError err;
        dbus_error_init(&err);

        uint32_t id;
        DBusMessageIter iter, array;
        dbus_message_iter_init(msg, &iter);

        dbus_message_iter_get_basic(&iter, &id);
        dbus_message_iter_next(&iter);
        dbus_message_iter_recurse(&iter, &array);

        uint8_t  buf[8];
        uint8_t  len = 0;
        int      type;

        while ((type = dbus_message_iter_get_arg_type(&array))
               != DBUS_TYPE_INVALID && len < 8) {
            dbus_message_iter_get_basic(&array, &buf[len]);
            dbus_message_iter_next(&array);
            len++;
        }

        can_drv_send(&g_can_drv, id, buf, len);

        DBusMessage *reply = dbus_message_new_method_return(msg);
        if (reply) {
            dbus_connection_send(conn, reply, NULL);
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusObjectPathVTable g_vtable = {
    .message_function = method_handler,
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    if (can_drv_init(&g_can_drv) != 0) {
        fprintf(stderr, "[can_service] CAN driver init failed\n");
        return 1;
    }
    printf("[can_service] CAN driver initialized\n");

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[can_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[can_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[can_service] request_name failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[can_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[can_service] register object failed\n");
        return 1;
    }

    printf("[can_service] D-Bus service registered: %s\n", SERVICE_NAME);

    Encoder_Data_t last_enc = {0};
    int            last_enc_valid = 0;

    while (g_running) {
        while (dbus_connection_read_write_dispatch(g_conn, 0))
            ;

        Encoder_Data_t enc;
        if (can_drv_get_encoder(&g_can_drv, &enc) == 0) {
            if (!last_enc_valid ||
                enc.count != last_enc.count ||
                enc.speed_rpm != last_enc.speed_rpm ||
                enc.angle != last_enc.angle) {

                emit_encoder_signal(g_conn, &enc);
                last_enc = enc;
                last_enc_valid = 1;
            }
        }

        usleep(50000);
    }

    printf("[can_service] shutting down...\n");
    can_drv_deinit(&g_can_drv);
    dbus_connection_unref(g_conn);
    return 0;
}