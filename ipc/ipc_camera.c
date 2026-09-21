/*
 * ipc_camera.c — 摄像头 IPC 实现
 *
 * 通过 D-Bus 信号 FrameReady 接收共享内存中的 JPEG 帧数据，
 * 回调通知 UI 层进行解码和渲染。
 */
#include "ipc_camera.h"
#include "lvgl_dbus_protocol.h"
#include "ipc_base.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_NAME "/lvgl_camera_frame"

static DBusConnection *g_conn = NULL;
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static ipc_camera_frame_cb_t g_user_cb = NULL;
static void *g_user_data = NULL;
static int   g_service_connected = 0;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, CAMERA_IFACE_NAME, CAMERA_SIGNAL_FRAME)) {
        if (g_user_cb) {
            dbus_uint32_t seq = 0, size = 0, width = 0, height = 0;
            dbus_uint64_t timestamp = 0;
            dbus_message_get_args(msg, NULL,
                DBUS_TYPE_UINT32, &seq,
                DBUS_TYPE_UINT32, &size,
                DBUS_TYPE_UINT64, &timestamp,
                DBUS_TYPE_UINT32, &width,
                DBUS_TYPE_UINT32, &height,
                DBUS_TYPE_INVALID);
            g_user_cb(seq, size, timestamp, width, height, g_user_data);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    /* 监听服务 NameOwnerChanged：camera_service 重启时自动重连共享内存 */
    if (dbus_message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged")) {
        const char *nm = NULL, *old_owner = NULL, *new_owner = NULL;
        if (dbus_message_get_args(msg, NULL,
                DBUS_TYPE_STRING, &nm,
                DBUS_TYPE_STRING, &old_owner,
                DBUS_TYPE_STRING, &new_owner,
                DBUS_TYPE_INVALID)) {
            if (nm && strcmp(nm, CAMERA_SERVICE_NAME) == 0) {
                if (new_owner && new_owner[0] != '\0') {
                    g_service_connected = 1;
                    /* 重新打开共享内存（服务端可能重建了 shm 对象） */
                    if (g_shm_ptr && g_shm_ptr != MAP_FAILED) {
                        munmap(g_shm_ptr, g_frame_size);
                        g_shm_ptr = NULL;
                    }
                    if (g_shm_fd >= 0) { close(g_shm_fd); g_shm_fd = -1; }
                    g_shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
                    if (g_shm_fd >= 0) {
                        g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ,
                                         MAP_SHARED, g_shm_fd, 0);
                        if (g_shm_ptr == MAP_FAILED) {
                            close(g_shm_fd);
                            g_shm_fd = -1;
                        }
                    }
                    printf("[ipc_camera] service appeared, shared memory reopened\n");
                } else {
                    g_service_connected = 0;
                    printf("[ipc_camera] service disappeared\n");
                }
            }
        }
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

    g_conn = ipc_base_connect(bus_address);
    if (!g_conn) return -1;

    const char *rule = "type='signal',interface='" CAMERA_IFACE_NAME "',member='" CAMERA_SIGNAL_FRAME "'";
    dbus_bus_add_match(g_conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_camera] add_match failed: %s\n", err.message);
        dbus_error_free(&err);
        ipc_base_disconnect(&g_conn, NULL);
        return -1;
    }

    /* 监听 NameOwnerChanged 实现服务发现与重连 */
    ipc_base_watch_service(g_conn, CAMERA_SERVICE_NAME);

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_camera] add_filter failed\n");
        dbus_connection_unref(g_conn);
        g_conn = NULL;
        return -1;
    }

    /* 等待 camera_service 上线 */
    if (ipc_base_wait_for_service(g_conn, CAMERA_SERVICE_NAME, 25) == 0)
        g_service_connected = 1;
    else
        fprintf(stderr, "[ipc_camera] WARNING: camera_service not found after 500ms\n");

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
        CAMERA_SERVICE_NAME, CAMERA_OBJECT_PATH, CAMERA_IFACE_NAME, method);
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
    return send_method(CAMERA_METHOD_START);
}

int ipc_camera_stop(void)
{
    return send_method(CAMERA_METHOD_STOP);
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
    ipc_base_disconnect(&g_conn, filter_cb);
    printf("[ipc_camera] deinitialized\n");
}