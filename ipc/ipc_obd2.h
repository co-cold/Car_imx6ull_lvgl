#ifndef IPC_OBD2_H
#define IPC_OBD2_H

/*
 * ipc_obd2.h — OBD-II IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 从 obd2_service 订阅车速/RPM/节气门数据，
 *       包含编码器兼容数据（Encoder_Data_t）和完整诊断数据。
 */

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "obd2_drv.h"

typedef void (*ipc_obd2_data_cb_t)(const Encoder_Data_t *data, void *user_data);

int  ipc_obd2_init(const char *bus_address,
                   ipc_obd2_data_cb_t cb, void *user_data);

int  ipc_obd2_get_data(Encoder_Data_t *data);

void ipc_obd2_dispatch(int timeout_ms);

void ipc_obd2_deinit(void);

#endif