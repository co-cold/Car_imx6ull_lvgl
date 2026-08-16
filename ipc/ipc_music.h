#ifndef IPC_MUSIC_H
#define IPC_MUSIC_H

#include <stdint.h>

int ipc_music_init(const char *bus_address);

void ipc_music_deinit(void);

void ipc_music_dispatch(int timeout_ms);

int ipc_music_play_audio(const char *file);

int ipc_music_pause(void);

int ipc_music_resume(void);

int ipc_music_stop(void);

int ipc_music_seek(double seconds);

int ipc_music_set_volume(float volume);

double ipc_music_get_position(void);

double ipc_music_get_duration(void);

int ipc_music_get_state(void);

float ipc_music_get_volume(void);

#endif