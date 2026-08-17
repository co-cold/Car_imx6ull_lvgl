#ifndef IPC_MEDIA_H
#define IPC_MEDIA_H

#include <stdint.h>

typedef void (*ipc_media_complete_cb_t)(void *user_data);
typedef void (*ipc_media_frame_cb_t)(void *user_data);

int ipc_media_init(const char *bus_address);

void ipc_media_deinit(void);

void ipc_media_dispatch(int timeout_ms);

/* 播放完成回调（音乐播放结束通知） */
void ipc_media_set_complete_callback(ipc_media_complete_cb_t cb, void *user_data);

/* 视频帧就绪回调 */
void ipc_media_set_frame_callback(ipc_media_frame_cb_t cb, void *user_data);

/* ─── 音频播放 ─── */
int ipc_media_play_audio(const char *file);
int ipc_media_pause(void);
int ipc_media_resume(void);
int ipc_media_stop(void);
int ipc_media_seek(double seconds);
int ipc_media_set_volume(float volume);

/* ─── 视频播放 ─── */
int ipc_media_play_video(const char *file, int width, int height);

/* ─── 合并查询：一次调用获取 state + position + duration ─── */
int ipc_media_get_playback_info(int *state, double *position, double *duration);

/* ─── 单独查询（兼容旧接口） ─── */
double ipc_media_get_position(void);
double ipc_media_get_duration(void);
int    ipc_media_get_state(void);
float  ipc_media_get_volume(void);

/* ─── 视频帧获取 ─── */
int ipc_media_get_video_frame(uint8_t *dst, int max_size);

#endif