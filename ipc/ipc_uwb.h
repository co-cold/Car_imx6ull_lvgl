#ifndef IPC_UWB_H
#define IPC_UWB_H

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "uwb_driver.h"

typedef void (*ipc_uwb_data_cb_t)(const Uwb_Data_t *data, void *user_data);

int  ipc_uwb_init(const char *bus_address,
                  ipc_uwb_data_cb_t cb, void *user_data);

int  ipc_uwb_get_data(Uwb_Data_t *data);

void ipc_uwb_dispatch(int timeout_ms);

void ipc_uwb_deinit(void);

#endif