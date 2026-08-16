#ifndef IPC_CAN_H
#define IPC_CAN_H

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "can_drv.h"

typedef void (*ipc_can_encoder_cb_t)(const Encoder_Data_t *enc, void *user_data);

int ipc_can_init(const char *bus_address,
                 ipc_can_encoder_cb_t cb, void *user_data);

int ipc_can_get_encoder(Encoder_Data_t *enc);

int ipc_can_send_frame(uint32_t id, const uint8_t *data, uint8_t len);

void ipc_can_dispatch(int timeout_ms);

void ipc_can_deinit(void);

#endif