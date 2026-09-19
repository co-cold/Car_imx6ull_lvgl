#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>

#include "uwb_driver.h"

#define SERVICE_NAME   "com.lvgl.demo.UWB"
#define OBJECT_PATH    "/com/lvgl/demo/UWB"
#define INTERFACE_NAME "com.lvgl.demo.UWB"
#define BUS_ADDRESS    "unix:path=/tmp/lvgl-dbus-session"
#define DEFAULT_DEVICE "/dev/ttyUSB0"
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
        OBJECT_PATH, INTERFACE_NAME, "UwbDataUpdated");
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

    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    const char *device   = DEFAULT_DEVICE;
    int         baudrate = DEFAULT_BAUD;

    if (argc >= 2) device   = argv[1];
    if (argc >= 3) baudrate = atoi(argv[2]);

    if (uwb_drv_init(&g_uwb_drv, device, baudrate) != 0) {
        fprintf(stderr, "[uwb_service] UWB driver init failed\n");
        return 1;
    }

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(BUS_ADDRESS, &err);
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

    int ret = dbus_bus_request_name(g_conn, SERVICE_NAME,
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

    if (!dbus_connection_register_object_path(g_conn, OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[uwb_service] register object failed\n");
        return 1;
    }

    printf("[uwb_service] D-Bus service registered: %s\n", SERVICE_NAME);

    Uwb_Data_t last_data;
    int        last_valid = 0;

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

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

        usleep(20000);
    }

    printf("[uwb_service] shutting down...\n");
    uwb_drv_deinit(&g_uwb_drv);
    dbus_connection_unref(g_conn);
    return 0;
}