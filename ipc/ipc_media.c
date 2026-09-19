#include "ipc_media.h"
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

/* 视频共享内存 */
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static int   g_video_width = 800;
static int   g_video_height = 450;

/* 回调 */
static ipc_media_complete_cb_t g_complete_cb = NULL;
static void *g_complete_user_data = NULL;
static ipc_media_frame_cb_t g_frame_cb = NULL;
static void *g_frame_user_data = NULL;

/* ─── D-Bus 信号过滤器 ─── */
static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, MEDIA_INTERFACE, "PlaybackComplete")) {
        if (g_complete_cb)
            g_complete_cb(g_complete_user_data);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    if (dbus_message_is_signal(msg, MEDIA_INTERFACE, "FrameReady")) {
        if (g_frame_cb)
            g_frame_cb(g_frame_user_data);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/* ─── 共享内存 ─── */
static int open_shm(void)
{
    int new_frame_size = g_video_width * g_video_height * 2;

    /* 如果已经映射且尺寸没变，直接复用 */
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED && g_frame_size == new_frame_size) {
        return 0;
    }

    /* 尺寸变了或首次映射，需要重新映射 */
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED) {
        munmap(g_shm_ptr, g_frame_size);
        g_shm_ptr = NULL;
    }
    if (g_shm_fd >= 0) {
        close(g_shm_fd);
        g_shm_fd = -1;
    }

    g_frame_size = new_frame_size;
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

/* ─── D-Bus 发送辅助 ─── */
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
        g_conn, msg, 100, &err);
    dbus_message_unref(msg);

    if (!reply || dbus_error_is_set(&err)) {
        dbus_error_free(&err);
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
        g_conn, msg, 100, &err);
    dbus_message_unref(msg);

    if (!reply || dbus_error_is_set(&err)) {
        dbus_error_free(&err);
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

/* ─── 公开 API ─── */
int ipc_media_init(const char *bus_address)
{
    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_media] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_media] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" MEDIA_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_media] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_media] add_filter failed\n");
        return -1;
    }

    printf("[ipc_media] initialized\n");

    /* 等待 media_service 注册 D-Bus 服务名 */
    {
        int retries = 0;
        while (retries < 25) {
            DBusMessage *m = dbus_message_new_method_call(
                "org.freedesktop.DBus", "/org/freedesktop/DBus",
                "org.freedesktop.DBus", "NameHasOwner");
            if (m) {
                const char *name = MEDIA_SERVICE_NAME;
                dbus_message_append_args(m, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
                DBusError e;
                dbus_error_init(&e);
                DBusMessage *r = dbus_connection_send_with_reply_and_block(g_conn, m, 50, &e);
                if (r) {
                    dbus_bool_t has_owner = FALSE;
                    dbus_message_get_args(r, &e, DBUS_TYPE_BOOLEAN, &has_owner, DBUS_TYPE_INVALID);
                    dbus_message_unref(r);
                    if (has_owner) {
                        dbus_message_unref(m);
                        break;
                    }
                }
                dbus_error_free(&e);
                dbus_message_unref(m);
            }
            usleep(20000);
            retries++;
        }
        if (retries >= 25)
            fprintf(stderr, "[ipc_media] WARNING: media_service not found after 500ms\n");
    }

    return 0;
}

void ipc_media_deinit(void)
{
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED)
        munmap(g_shm_ptr, g_frame_size);
    if (g_shm_fd >= 0)
        close(g_shm_fd);
    g_shm_ptr = NULL;
    g_shm_fd = -1;

    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    printf("[ipc_media] deinitialized\n");
}

void ipc_media_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_media_set_complete_callback(ipc_media_complete_cb_t cb, void *user_data)
{
    g_complete_cb = cb;
    g_complete_user_data = user_data;
}

void ipc_media_set_frame_callback(ipc_media_frame_cb_t cb, void *user_data)
{
    g_frame_cb = cb;
    g_frame_user_data = user_data;
}

/* ─── 音频播放 ─── */
int ipc_media_play_audio(const char *file)
{
    return send_string("PlayAudio", file);
}

int ipc_media_pause(void)          { return send_simple("Pause"); }
int ipc_media_resume(void)         { return send_simple("Resume"); }
int ipc_media_stop(void)           { return send_simple("Stop"); }
int ipc_media_seek(double seconds) { return send_double("Seek", seconds); }

/* ─── 视频播放 ─── */
int ipc_media_play_video(const char *file, int width, int height)
{
    g_video_width = width;
    g_video_height = height;

    /* 不关闭共享内存！media_service 端也不再 shm_unlink，
     * 两边复用同一个共享内存对象，避免竞态条件导致 UI 侧
     * 打开旧对象后永远读不到新数据。 */

    return send_string_uint_uint("PlayVideo", file, (uint32_t)width, (uint32_t)height);
}

/* ─── 合并查询：一次 D-Bus 调用返回 state + position + duration ─── */
int ipc_media_get_playback_info(int *state, double *position, double *duration)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        MEDIA_SERVICE_NAME, MEDIA_OBJECT_PATH, MEDIA_INTERFACE, "GetPlaybackInfo");
    if (!msg) return -1;

    DBusError err;
    dbus_error_init(&err);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(
        g_conn, msg, 100, &err);
    dbus_message_unref(msg);

    if (!reply || dbus_error_is_set(&err)) {
        dbus_error_free(&err);
        return -1;
    }

    dbus_int32_t s = 0;
    double pos = 0.0;
    double dur = 0.0;
    if (dbus_message_get_args(reply, &err,
            DBUS_TYPE_INT32, &s,
            DBUS_TYPE_DOUBLE, &pos,
            DBUS_TYPE_DOUBLE, &dur,
            DBUS_TYPE_INVALID)) {
        if (state)    *state = (int)s;
        if (position) *position = pos;
        if (duration) *duration = dur;
    }
    dbus_message_unref(reply);
    return 0;
}

/* ─── 单独查询（兼容旧接口） ─── */
double ipc_media_get_position(void) { double v = 0.0; call_double("GetPosition", &v); return v; }
double ipc_media_get_duration(void) { double v = 0.0; call_double("GetDuration", &v); return v; }
int    ipc_media_get_state(void)    { int v = 0;    call_int("GetState", &v);       return v; }

/* ─── 视频帧获取 ─── */
int ipc_media_get_video_frame(uint8_t *dst, int max_size)
{
    if (g_shm_fd < 0)
        open_shm();
    if (!g_shm_ptr || !dst) return -1;

    int copy_size = g_frame_size < max_size ? g_frame_size : max_size;
    memcpy(dst, g_shm_ptr, copy_size);
    return 0;
}