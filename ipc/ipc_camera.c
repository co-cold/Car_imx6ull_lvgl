#include "ipc_camera.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CAMERA_SERVICE_NAME "com.lvgl.demo.Camera"
#define CAMERA_OBJECT_PATH  "/com/lvgl/demo/Camera"
#define CAMERA_INTERFACE    "com.lvgl.demo.Camera"
#define SHM_NAME            "/lvgl_camera_frame"

static DBusConnection *g_conn = NULL;
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static ipc_camera_frame_cb_t g_user_cb = NULL;
static void *g_user_data = NULL;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, CAMERA_INTERFACE, "FrameReady")) {
        if (g_user_cb)
            g_user_cb(g_user_data);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int ipc_camera_init(const char *bus_address,
                    int width, int height,
                    ipc_camera_frame_cb_t cb, void *user_data)
{
    DBusError err;
    dbus_error_init(&err);

    g_frame_size = width * height * 2;

    g_conn = dbus_connection_open(bus_address, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_camera] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_camera] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    const char *rule = "type='signal',interface='" CAMERA_INTERFACE "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_camera] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        return -1;
    }

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_camera] add_filter failed\n");
        return -1;
    }

    g_shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (g_shm_fd < 0) {
        fprintf(stderr, "[ipc_camera] shm_open failed, will retry on start\n");
    } else {
        g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ,
                         MAP_SHARED, g_shm_fd, 0);
        if (g_shm_ptr == MAP_FAILED) {
            fprintf(stderr, "[ipc_camera] mmap failed\n");
            close(g_shm_fd);
            g_shm_fd = -1;
        }
    }

    g_user_cb   = cb;
    g_user_data = user_data;

    printf("[ipc_camera] initialized, %dx%d, frame_size=%d\n",
           width, height, g_frame_size);
    return 0;
}

static int send_method(const char *method)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        CAMERA_SERVICE_NAME, CAMERA_OBJECT_PATH, CAMERA_INTERFACE, method);
    if (!msg) return -1;

    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
    return 0;
}

int ipc_camera_start(void)
{
    if (g_shm_fd < 0) {
        g_shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
        if (g_shm_fd >= 0) {
            g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ,
                             MAP_SHARED, g_shm_fd, 0);
            if (g_shm_ptr == MAP_FAILED) {
                close(g_shm_fd);
                g_shm_fd = -1;
            }
        }
    }
    return send_method("Start");
}

int ipc_camera_stop(void)
{
    return send_method("Stop");
}

int ipc_camera_get_frame(uint8_t *dst, int max_size)
{
    if (!g_shm_ptr || !dst) return -1;

    int copy_size = g_frame_size < max_size ? g_frame_size : max_size;
    memcpy(dst, g_shm_ptr, copy_size);
    return 0;
}

void ipc_camera_dispatch(int timeout_ms)
{
    if (g_conn)
        dbus_connection_read_write_dispatch(g_conn, timeout_ms);
}

void ipc_camera_deinit(void)
{
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED)
        munmap(g_shm_ptr, g_frame_size);
    if (g_shm_fd >= 0)
        close(g_shm_fd);
    if (g_conn) {
        dbus_connection_remove_filter(g_conn, filter_cb, NULL);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
    printf("[ipc_camera] deinitialized\n");
}