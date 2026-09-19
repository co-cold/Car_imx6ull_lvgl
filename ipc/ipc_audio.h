#ifndef IPC_AUDIO_H
#define IPC_AUDIO_H

#include <stdbool.h>

bool ipc_audio_init(const char *bus_address);
void ipc_audio_deinit(void);
void ipc_audio_dispatch(int timeout_ms);

int ipc_audio_set_volume(float volume);
float ipc_audio_get_volume(void);

#endif