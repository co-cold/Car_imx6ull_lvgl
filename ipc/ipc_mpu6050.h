#ifndef IPC_MPU6050_H
#define IPC_MPU6050_H

/*
 * ipc_mpu6050.h — MPU6050 IPC 通信接口（D-Bus 客户端）
 *
 * 功能：通过 D-Bus 从 mpu6050_service 订阅加速度/角速度数据，
 *       及驾驶行为检测结果（急刹/急加速/转向）。
 */

#include <stdint.h>
#include <pthread.h>
#include <dbus/dbus.h>

#include "mpu6050_drv.h"

typedef void (*ipc_mpu6050_data_cb_t)(const Mpu6050_Data_t *data, void *user_data);

int  ipc_mpu6050_init(const char *bus_address,
                      ipc_mpu6050_data_cb_t cb, void *user_data);

int  ipc_mpu6050_get_data(Mpu6050_Data_t *data);

void ipc_mpu6050_dispatch(int timeout_ms);

void ipc_mpu6050_deinit(void);

#endif