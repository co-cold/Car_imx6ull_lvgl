#include "ipc_music.h"
#include <stdio.h>
#include <dbus/dbus.h>

#define MEDIA_SERVICE_NAME "com.lvgl.demo.Media"
#define MEDIA_OBJECT_PATH  "/com/lvgl/demo/Media"
#define MEDIA_INTERFACE    "com.lvgl.demo.Media"

static DBusConnection *g_conn = NULL;

static int send_simple(const char *method)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    return 0;
}

static int send_string(const char *method, const char *arg)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    dbus_message_append_args(msg, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    return 0;
}

static int send_double(const char *method, double val)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    dbus_message_append_args(msg, DBUS_TYPE_DOUBLE, &val, DBUS_TYPE_INVALID);
    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    return 0;
}

static int call_int(const char *method, int *out)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    DBusError err;
    dbus_error_init(&err);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(
        g_conn, msg, 5000, &err);
    dbus_message_unref(msg);

    if (!reply || dbus_error_is_set(&err)) {
        if (dbus_error_is_set(&err)) {
            fprintf(stderr, "[ipc_music] %s error: %s\n", method, err.message);
            dbus_error_free(&err);
        }
        return -1;
    }

    dbus_int32_t val = 0;
    if (dbus_message_get_args(reply, &err,
            DBUS_TYPE_INT32, &val, DBUS_TYPE_INVALID)) {
        if (out) *out = (int)val;
    }
    dbus_message_unref(reply);
    return 0;
}

static int call_double(const char *method, double *out)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    DBusError err;
    dbus_error_init(&err);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(
        g_conn, msg, 5000, &err);
    dbus_message_unref(msg);

    if (!reply || dbus_error_is_set(&err)) {
        if (dbus_error_is_set(&err)) {
            fprintf(stderr, "[ipc_music] %s error: %s\n", method, err.message);
            dbus_error_free(&err);
        }
        return -1;
    }

    double val = 0.0;
    if (dbus_message_get_args(reply, &err,
            DBUS_TYPE_DOUBLE, &val, DBUS_TYPE_INVALID)) {
        if (out) *out = val;
    }
    dbus_message_unref(reply);
    return 0;
}

int ipc_music_init(const char *bus_address)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_music] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_music] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    printf("[ipc_music] initialized\n");
    return 0;
}

int ipc_music_play_audio(const char *file)
{
    return send_string("PlayAudio", file);
}

int ipc_music_pause(void)          { return send_simple("Pause"); }
int ipc_music_resume(void)         { return send_simple("Resume"); }
int ipc_music_stop(void)           { return send_simple("Stop"); }
int ipc_music_seek(double seconds) { return send_double("Seek", seconds); }
int ipc_music_set_volume(float v)  { return send_double("SetVolume", (double)v); }

double ipc_music_get_position(void) { double v = 0.0; call_double("GetPosition", &v); return v; }
double ipc_music_get_duration(void) { double v = 0.0; call_double("GetDuration", &v); return v; }
int    ipc_music_get_state(void)    { int v = 0;    call_int("GetState", &v);       return v; }
float  ipc_music_get_volume(void)   { double v = 1.0; call_double("GetVolume", &v);  return (float)v; }

void ipc_music_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_music_deinit(void)
{
    if (g_conn) {
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    printf("[ipc_music] deinitialized\n");
}