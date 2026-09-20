#ifndef IPC_CAN_H
#define IPC_CAN_H

/*
 * ipc_can.h — CAN 总线 IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 从 obd2_service 订阅编码器信号，
 *       同时支持发送 CAN 帧用于 ECU 控制（如设置档位）。
 */

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "obd2_drv.h"

typedef void (*ipc_can_encoder_cb_t)(const Encoder_Data_t *enc, void *user_data);

int ipc_can_init(const char *bus_address,
                 ipc_can_encoder_cb_t cb, void *user_data);

int ipc_can_get_encoder(Encoder_Data_t *enc);

int ipc_can_send_frame(uint32_t id, const uint8_t *data, uint8_t len);

void ipc_can_dispatch(int timeout_ms);

void ipc_can_deinit(void);

#endif