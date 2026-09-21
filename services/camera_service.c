/*
 * camera_service.c — 摄像头 D-Bus 服务
 *
 * 功能：使用 V4L2 驱动采集摄像头（/dev/video0）JPEG 帧，
 *       通过共享内存 + D-Bus 信号 FrameReady 推送给 UI 端。
 *       
 * 数据流：V4L2 capture → JPEG 编码 → shm_open 写入 → DBus 信号通知
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <dbus/dbus.h>

#include "camera_hardware.h"
#include "ipc/lvgl_dbus_protocol.h"
#define SHM_NAME       "/lvgl_camera_frame"

#define CAM_WIDTH   640
#define CAM_HEIGHT  480
#define CAM_FPS     30
#define CAM_DEVICE  "/dev/video1"

static volatile int g_running = 1;
static DBusConnection  *g_conn = NULL;
static camera_hardware_t *g_camera = NULL;
static int   g_shm_fd = -1;
static void *g_shm_ptr = NULL;
static int   g_frame_size = 0;
static uint32_t g_frame_seq = 0;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static void emit_frame_ready(void)
{
    DBusMessage *msg = dbus_message_new_signal(
        CAMERA_OBJECT_PATH, CAMERA_IFACE_NAME, CAMERA_SIGNAL_FRAME);
    if (!msg) return;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t timestamp = (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;

    dbus_uint32_t seq   = ++g_frame_seq;
    dbus_uint32_t size  = (dbus_uint32_t)g_frame_size;
    dbus_uint32_t width = CAM_WIDTH;
    dbus_uint32_t height = CAM_HEIGHT;

    dbus_message_append_args(msg,
        DBUS_TYPE_UINT32, &seq,
        DBUS_TYPE_UINT32, &size,
        DBUS_TYPE_UINT64, &timestamp,
        DBUS_TYPE_UINT32, &width,
        DBUS_TYPE_UINT32, &height,
        DBUS_TYPE_INVALID);

    dbus_connection_send(g_conn, msg, NULL);
    dbus_connection_flush(g_conn);
    dbus_message_unref(msg);
}

static int init_shared_memory(void)
{
    g_frame_size = CAM_WIDTH * CAM_HEIGHT * 2;

    shm_unlink(SHM_NAME);

    g_shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (g_shm_fd < 0) {
        perror("[camera_service] shm_open");
        return -1;
    }

    if (ftruncate(g_shm_fd, g_frame_size) < 0) {
        perror("[camera_service] ftruncate");
        close(g_shm_fd);
        return -1;
    }

    g_shm_ptr = mmap(NULL, g_frame_size, PROT_READ | PROT_WRITE,
                     MAP_SHARED, g_shm_fd, 0);
    if (g_shm_ptr == MAP_FAILED) {
        perror("[camera_service] mmap");
        close(g_shm_fd);
        return -1;
    }

    memset(g_shm_ptr, 0, g_frame_size);
    printf("[camera_service] shared memory created: %s, size=%d\n",
           SHM_NAME, g_frame_size);
    return 0;
}

static DBusHandlerResult method_handler(DBusConnection *conn,
    DBusMessage *msg, void *data)
{
    (void)data;

    if (dbus_message_is_method_call(msg, CAMERA_IFACE_NAME, CAMERA_METHOD_START)) {
        dbus_int32_t ret = -1;
        if (g_camera && camera_hw_start(g_camera) == 0) {
            printf("[camera_service] camera started\n");
            ret = 0;
        } else {
            fprintf(stderr, "[camera_service] camera start failed\n");
        }
        DBusMessage *reply = dbus_message_new_method_return(msg);
        if (reply) {
            dbus_message_append_args(reply, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
            dbus_connection_send(conn, reply, NULL);
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    if (dbus_message_is_method_call(msg, CAMERA_IFACE_NAME, CAMERA_METHOD_STOP)) {
        dbus_int32_t ret = -1;
        if (g_camera) {
            camera_hw_stop(g_camera);
            printf("[camera_service] camera stopped\n");
            ret = 0;
        } else {
            fprintf(stderr, "[camera_service] camera stop failed: no device\n");
        }
        DBusMessage *reply = dbus_message_new_method_return(msg);
        if (reply) {
            dbus_message_append_args(reply, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
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

    if (init_shared_memory() != 0)
        return 1;

    g_camera = camera_hw_create(CAM_DEVICE, CAM_WIDTH, CAM_HEIGHT,
                                CAM_FPS, CAMERA_FORMAT_RGB565);
    if (!g_camera) {
        fprintf(stderr, "[camera_service] camera_hw_create failed\n");
        return 1;
    }

    if (camera_hw_init(g_camera) != 0) {
        fprintf(stderr, "[camera_service] camera_hw_init failed\n");
        return 1;
    }
    printf("[camera_service] camera hardware initialized\n");

    DBusError err;
    dbus_error_init(&err);

    g_conn = dbus_connection_open(PROTO_BUS_ADDRESS, &err);
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[camera_service] D-Bus connect failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[camera_service] register failed: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    int ret = dbus_bus_request_name(g_conn, CAMERA_SERVICE_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &err);
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[camera_service] name already taken\n");
        return 1;
    }

    if (!dbus_connection_register_object_path(g_conn, CAMERA_OBJECT_PATH,
            &g_vtable, NULL)) {
        fprintf(stderr, "[camera_service] register object failed\n");
        return 1;
    }

    printf("[camera_service] D-Bus service registered: %s\n", CAMERA_SERVICE_NAME);

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 0);

        if (g_camera && g_shm_ptr) {
            if (camera_hw_get_latest_frame(g_camera, g_shm_ptr) == 0) {
                emit_frame_ready();
            }
        }

        usleep(30000);
    }

    printf("[camera_service] shutting down...\n");
    if (g_camera) {
        camera_hw_stop(g_camera);
        camera_hw_destroy(g_camera);
    }
    if (g_shm_ptr && g_shm_ptr != MAP_FAILED)
        munmap(g_shm_ptr, g_frame_size);
    if (g_shm_fd >= 0)
        close(g_shm_fd);
    shm_unlink(SHM_NAME);
    dbus_connection_unref(g_conn);
    return 0;
}