#ifndef IPC_UWB_H
#define IPC_UWB_H

/*
 * ipc_uwb.h — UWB 定位 IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 从 uwb_service 订阅距离数据信号，
 *       回调通知上层（sensor_fusion / UI）。
 *       支持设备状态变化信号（connected/disconnected）。
 */

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "uwb_driver.h"

typedef void (*ipc_uwb_data_cb_t)(const Uwb_Data_t *data, void *user_data);
typedef void (*ipc_uwb_status_cb_t)(int connected, void *user_data);

int  ipc_uwb_init(const char *bus_address,
                  ipc_uwb_data_cb_t data_cb, void *user_data);

int  ipc_uwb_get_data(Uwb_Data_t *data);

int  ipc_uwb_is_connected(void);

void ipc_uwb_set_status_callback(ipc_uwb_status_cb_t cb, void *user_data);

void ipc_uwb_dispatch(int timeout_ms);

void ipc_uwb_deinit(void);

#endif