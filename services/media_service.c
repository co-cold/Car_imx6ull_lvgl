#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dbus/dbus.h>

#include "player_core.h"

#define SERVICE_NAME   "com.lvgl.demo.Media"
#define OBJECT_PATH    "/com/lvgl/demo/Media"
#define INTERFACE_NAME "com.lvgl.demo.Media"
#define BUS_ADDRESS    "unix:path=/tmp/lvgl-dbus-session"
#define SHM_NAME       "/lvgl_video_frame"

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static PlayerCore *g_pc = NULL;
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static int   g_video_width = 800;
static int   g_video_height = 450;
static int   g_video_mode = 0;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_frame_ready(void)
{
    DBusMessage *msg = dbus_message_new_signal(
        OBJECT_PATH, INTERFACE_NAME, "FrameReady");
    if (!msg) return;
    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
}

static void emit_playback_complete(void)
{
    DBusMessage *msg = dbus_message_new_signal(
        OBJECT_PATH, INTERFACE_NAME, "PlaybackComplete");
    if (!msg) return;
    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    printf("[media_service] PlaybackComplete signal emitted\n");
}

static int init_shared_memory(int width, int height)
{
    int new_frame_size = width * height * 2;

    /* 先解除旧映射，关闭旧fd，但 ***不*** shm_unlink */
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED) {
        munmap(g_shm_ptr, g_frame_size);
        g_shm_ptr = NULL;
    }
    if (g_shm_fd >= 0) {
        close(g_shm_fd);
        g_shm_fd = -1;
    }

    g_video_width  = width;
    g_video_height = height;
    g_frame_size   = new_frame_size;

    /* 尝试打开已有的共享内存（不 unlink，复用同一对象） */
    g_shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (g_shm_fd < 0) {
        /* 首次创建 */
        g_shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (g_shm_fd < 0) {
            perror("[media_service] shm_open");
            return -1;
        }
    }

    if (ftruncate(g_shm_fd, g_frame_size) < 0) {
        perror("[media_service] ftruncate");
        close(g_shm_fd);
        g_shm_fd = -1;
        return -1;
    }

    g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ | PROT_WRITE,
                     MAP_SHARED, g_shm_fd, 0);
    if (g_shm_ptr == MAP_FAILED) {
        perror("[media_service] mmap");
        close(g_shm_fd);
        g_shm_fd = -1;
        return -1;
    }

    memset(g_shm_ptr, 0, g_frame_size);
    printf("[media_service] shared memory ready: %s, size=%d\n",
           SHM_NAME, g_frame_size);
    return 0;
}

static DBusHandlerResult method_handler(DBusConnection *conn,
    DBusMessage *msg, void *data)
{
    (void)data;
    DBusMessage *reply = NULL;
    const char *method = dbus_message_get_member(msg);

    if (!method)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    if (strcmp(method, "PlayAudio") == 0) {
        const char *file = NULL;
        dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &file, DBUS_TYPE_INVALID);
        int ret = -1;
        if (file && g_pc) {
            ret = player_core_play_audio(g_pc, file);
            g_video_mode = 0;
        }
        reply = dbus_message_new_method_return(msg);
        dbus_int32_t r = ret;
        dbus_message_append_args(reply, DBUS_TYPE_INT32, &r, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "PlayVideo") == 0) {
        const char *file = NULL;
        dbus_uint32_t w = 0, h = 0;
        dbus_message_get_args(msg, NULL,
            DBUS_TYPE_STRING, &file,
            DBUS_TYPE_UINT32, &w,
            DBUS_TYPE_UINT32, &h,
            DBUS_TYPE_INVALID);
        int ret = -1;
        if (file && g_pc) {
            init_shared_memory((int)w, (int)h);
            ret = player_core_play_video(g_pc, file, (int)w, (int)h);
            g_video_mode = (ret == 0) ? 1 : 0;
        }
        reply = dbus_message_new_method_return(msg);
        dbus_int32_t r = ret;
        dbus_message_append_args(reply, DBUS_TYPE_INT32, &r, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "Pause") == 0) {
        if (g_pc) player_core_pause(g_pc);
        reply = dbus_message_new_method_return(msg);
    }
    else if (strcmp(method, "Resume") == 0) {
        if (g_pc) player_core_resume(g_pc);
        reply = dbus_message_new_method_return(msg);
    }
    else if (strcmp(method, "Stop") == 0) {
        if (g_pc) player_core_stop(g_pc);
        g_video_mode = 0;
        reply = dbus_message_new_method_return(msg);
    }
    else if (strcmp(method, "Seek") == 0) {
        double sec = 0.0;
        dbus_message_get_args(msg, NULL, DBUS_TYPE_DOUBLE, &sec, DBUS_TYPE_INVALID);
        if (g_pc) player_core_seek(g_pc, sec);
        reply = dbus_message_new_method_return(msg);
    }
    else if (strcmp(method, "SetVolume") == 0) {
        double vol = 1.0;
        dbus_message_get_args(msg, NULL, DBUS_TYPE_DOUBLE, &vol, DBUS_TYPE_INVALID);
        if (g_pc) player_core_set_volume(g_pc, (float)vol);
        reply = dbus_message_new_method_return(msg);
    }
    else if (strcmp(method, "GetPosition") == 0) {
        double pos = g_pc ? player_core_get_position(g_pc) : 0.0;
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_DOUBLE, &pos, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "GetDuration") == 0) {
        double dur = g_pc ? player_core_get_duration(g_pc) : 0.0;
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_DOUBLE, &dur, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "GetState") == 0) {
        dbus_int32_t state = g_pc ? player_core_get_state(g_pc) : 0;
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_INT32, &state, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "GetVolume") == 0) {
        double vol = g_pc ? (double)player_core_get_volume(g_pc) : 1.0;
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_DOUBLE, &vol, DBUS_TYPE_INVALID);
    }
    else if (strcmp(method, "GetPlaybackInfo") == 0) {
        dbus_int32_t state = g_pc ? player_core_get_state(g_pc) : 0;
        double pos = g_pc ? player_core_get_position(g_pc) : 0.0;
        double dur = g_pc ? player_core_get_duration(g_pc) : 0.0;
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply,
            DBUS_TYPE_INT32, &state,
            DBUS_TYPE_DOUBLE, &pos,
            DBUS_TYPE_DOUBLE, &dur,
            DBUS_TYPE_INVALID);
    }
    else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    if (reply) {
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

    g_pc = player_core_create();
    if (!g_pc) {
        fprintf(stderr, "[media_service] player_core_create failed\n");
        return 1;
    }
    printf("[media_service] PlayerCore created\n");

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[media_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[media_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[media_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[media_service] register object failed\n");
        return 1;
    }

    printf("[media_service] D-Bus service registered: %s\n", SERVICE_NAME);

    int prev_audio_state = 0;

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

        if (g_pc && g_video_mode && g_shm_ptr) {
            double audio_time = player_core_get_position(g_pc);
            uint8_t *frame = player_core_get_video_frame_at_time(g_pc, audio_time);
            if (frame) {
                memcpy(g_shm_ptr, frame, g_frame_size);
                player_core_release_video_frame(g_pc);
                emit_frame_ready();
            }
        }

        if (g_pc && !g_video_mode) {
            int cur_state = player_core_get_state(g_pc);
            if (prev_audio_state == 1 && cur_state == 0) {
                emit_playback_complete();
            }
            prev_audio_state = cur_state;
        }

        usleep(30000);
    }

    printf("[media_service] shutting down...\n");
    if (g_pc) {
        player_core_stop(g_pc);
        player_core_destroy(g_pc);
        g_pc = NULL;
    }
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED)
        munmap(g_shm_ptr, g_frame_size);
    if (g_shm_fd >= 0)
        close(g_shm_fd);
    shm_unlink(SHM_NAME);
    dbus_connection_unref(g_conn);
    return 0;
}