#ifndef IPC_AUDIO_H
#define IPC_AUDIO_H

/*
 * ipc_audio.h — 音频 IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 与 audio_service 通信，
 *       控制系统音量（0.0-1.0）。
 */

#include <stdbool.h>

int ipc_audio_init(const char *bus_address);
void ipc_audio_deinit(void);
void ipc_audio_dispatch(int timeout_ms);

int ipc_audio_set_volume(float volume);
float ipc_audio_get_volume(void);

#endif