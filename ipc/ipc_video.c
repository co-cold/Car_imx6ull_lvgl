#include "ipc_video.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dbus/dbus.h>

#define MEDIA_SERVICE_NAME "com.lvgl.demo.Media"
#define MEDIA_OBJECT_PATH  "/com/lvgl/demo/Media"
#define MEDIA_INTERFACE    "com.lvgl.demo.Media"
#define SHM_NAME           "/lvgl_video_frame"

static DBusConnection *g_conn = NULL;
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static int   g_video_width = 800;
static int   g_video_height = 450;
static ipc_video_frame_cb_t g_frame_cb = NULL;
static void *g_user_data = NULL;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, MEDIA_INTERFACE, "FrameReady")) {
        if (g_frame_cb)
            g_frame_cb(g_user_data);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static int open_shm(void)
{
    g_frame_size = g_video_width * g_video_height * 2;
    g_shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (g_shm_fd < 0) return -1;

    g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ, MAP_SHARED, g_shm_fd, 0);
    if (g_shm_ptr == MAP_FAILED) {
        close(g_shm_fd);
        g_shm_fd = -1;
        return -1;
    }
    return 0;
}

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

static int send_string_uint_uint(const char *method, const char *s,
                                  uint32_t u1, uint32_t u2)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, method);
    if (!msg) return -1;

    dbus_message_append_args(msg,
        DBUS_TYPE_STRING, &s,
        DBUS_TYPE_UINT32, &u1,
        DBUS_TYPE_UINT32, &u2,
        DBUS_TYPE_INVALID);
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
            fprintf(stderr, "[ipc_video] %s error: %s\n", method, err.message);
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
            fprintf(stderr, "[ipc_video] %s error: %s\n", method, err.message);
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

int ipc_video_init(const char *bus_address,
                   ipc_video_frame_cb_t frame_cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_video] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_video] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" MEDIA_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_video] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_video] add_filter failed\n");
        return -1;
    }

    g_frame_cb   = frame_cb;
    g_user_data  = user_data;

    printf("[ipc_video] initialized\n");
    return 0;
}

int ipc_video_play(const char *file, int width, int height)
{
    g_video_width  = width;
    g_video_height = height;
    if (g_shm_fd < 0)
        open_shm();
    return send_string_uint_uint("PlayVideo", file,
                                 (uint32_t)width, (uint32_t)height);
}

int ipc_video_pause(void)          { return send_simple("Pause"); }
int ipc_video_resume(void)         { return send_simple("Resume"); }
int ipc_video_stop(void)           { return send_simple("Stop"); }
int ipc_video_seek(double seconds) { return send_double("Seek", seconds); }
int ipc_video_set_volume(float v)  { return send_double("SetVolume", (double)v); }

double ipc_video_get_position(void) { double v = 0.0; call_double("GetPosition", &v); return v; }
double ipc_video_get_duration(void) { double v = 0.0; call_double("GetDuration", &v); return v; }
int    ipc_video_get_state(void)    { int v = 0;    call_int("GetState", &v);       return v; }
float  ipc_video_get_volume(void)   { double v = 1.0; call_double("GetVolume", &v);  return (float)v; }

int ipc_video_get_frame(uint8_t *dst, int max_size)
{
    if (!g_shm_ptr || !dst) return -1;

    int copy_size = g_frame_size < max_size ? g_frame_size : max_size;
    memcpy(dst, g_shm_ptr, copy_size);
    return 0;
}

void ipc_video_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_video_deinit(void)
{
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED)
        munmap(g_shm_ptr, g_frame_size);
    if (g_shm_fd >= 0)
        close(g_shm_fd);
    g_shm_ptr = NULL;
    g_shm_fd  = -1;

    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    printf("[ipc_video] deinitialized\n");
}