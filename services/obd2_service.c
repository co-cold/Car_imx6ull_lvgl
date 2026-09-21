/*
 * obd2_service.c — OBD-II 诊断 D-Bus 服务
 *
 * 功能：通过 SocketCAN 轮询 OBD-II ECU 获取 RPM/车速/节气门等数据，
 *       通过 D-Bus 信号 EncoderSignal 对外广播编码器兼容数据。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "obd2_drv.h"
#include "ipc/lvgl_dbus_protocol.h"

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static obd2_drv_t g_obd2_drv;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_vehicle_signal(DBusConnection *conn, const Encoder_Data_t *enc)
{
    DBusMessage *msg = dbus_message_new_signal(
        OBD2_OBJECT_PATH, OBD2_IFACE_NAME, OBD2_SIGNAL_ENCODER);
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

    dbus_int32_t ret = -1;
    DBusMessage *reply = dbus_message_new_method_return(msg);
    if (reply) {
        dbus_message_append_args(reply, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
        dbus_connection_send(conn, reply, NULL);
        dbus_connection_flush(conn);
        dbus_message_unref(reply);
    }
    return DBUS_HANDLER_RESULT_HANDLED;
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

    if (obd2_drv_init(&g_obd2_drv) != 0) {
        fprintf(stderr, "[obd2_service] OBD-II driver init failed\n");
        return 1;
    }

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(PROTO_BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[obd2_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[obd2_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, OBD2_SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[obd2_service] request_name failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[obd2_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, OBD2_OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[obd2_service] register object failed\n");
        return 1;
    }

    printf("[obd2_service] D-Bus service registered: %s\n", OBD2_SERVICE_NAME);

    Encoder_Data_t last_enc = {0};
    int            last_enc_valid = 0;

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

        Encoder_Data_t enc;
        if (obd2_drv_get_encoder(&g_obd2_drv, &enc) == 0) {
            if (!last_enc_valid ||
                enc.count     != last_enc.count ||
                enc.speed_rpm != last_enc.speed_rpm ||
                enc.angle     != last_enc.angle) {

                emit_vehicle_signal(g_conn, &enc);
                last_enc = enc;
                last_enc_valid = 1;
            }
        }

        usleep(50000);
    }

    printf("[obd2_service] shutting down...\n");
    obd2_drv_deinit(&g_obd2_drv);
    dbus_connection_unref(g_conn);
    return 0;
}