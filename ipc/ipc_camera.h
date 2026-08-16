#ifndef IPC_CAMERA_H
#define IPC_CAMERA_H

#include <stdint.h>
#include <dbus/dbus.h>

typedef void (*ipc_camera_frame_cb_t)(void *user_data);

int ipc_camera_init(const char *bus_address,
                    int width, int height,
                    ipc_camera_frame_cb_t cb, void *user_data);

int ipc_camera_start(void);

int ipc_camera_stop(void);

int ipc_camera_get_frame(uint8_t *dst, int max_size);

void ipc_camera_dispatch(int timeout_ms);

void ipc_camera_deinit(void);

#endif