#ifndef IPC_VIDEO_H
#define IPC_VIDEO_H

#include <stdint.h>

typedef void (*ipc_video_frame_cb_t)(void *user_data);

int ipc_video_init(const char *bus_address,
                   ipc_video_frame_cb_t frame_cb, void *user_data);

void ipc_video_deinit(void);

void ipc_video_dispatch(int timeout_ms);

int ipc_video_play(const char *file, int width, int height);

int ipc_video_pause(void);

int ipc_video_resume(void);

int ipc_video_stop(void);

int ipc_video_seek(double seconds);

int ipc_video_set_volume(float volume);

double ipc_video_get_position(void);

double ipc_video_get_duration(void);

int ipc_video_get_state(void);

float ipc_video_get_volume(void);

int ipc_video_get_frame(uint8_t *dst, int max_size);

#endif