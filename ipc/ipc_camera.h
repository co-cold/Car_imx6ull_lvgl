#ifndef IPC_CAMERA_H
#define IPC_CAMERA_H

/*
 * ipc_camera.h — 摄像头 IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 向 camera_service 订阅视频帧，
 *       接收编码后的 JPEG 帧数据，供 LVGL UI 渲染。
 *
 * 使用流程：
 *   ipc_camera_init(...)  → 连接 D-Bus + 注册信号回调
 *   ipc_camera_start()    → 开始接收帧
 *   ipc_camera_get_frame() → 获取最新帧数据
 *   ipc_camera_stop()     → 停止接收
 */

#include <stdint.h>
#include <dbus/dbus.h>

typedef void (*ipc_camera_frame_cb_t)(uint32_t seq, uint32_t size, uint64_t timestamp,
                                      uint32_t width, uint32_t height, void *user_data);

int ipc_camera_init(const char *bus_address,
                    int width, int height,
                    ipc_camera_frame_cb_t cb, void *user_data);

int ipc_camera_start(void);

int ipc_camera_stop(void);

int ipc_camera_get_frame(uint8_t *dst, int max_size);

void ipc_camera_dispatch(int timeout_ms);

void ipc_camera_deinit(void);

#endif