#ifndef _OBD2_DRV_H_
#define _OBD2_DRV_H_

/*
 * OBD-II 诊断驱动（模拟/实际）
 *
 * 功能：通过 UDP socket 获取模拟 OBD-II 数据（RPM、车速、节气门）。
 *       实际使用时替换为 CAN/ELM327 协议栈。
 *       输出编码器兼容数据 + 完整 OBD-II 数据两套接口。
 *
 * 数据更新率：约 10Hz（UDP 模拟）
 */

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

// 编码器兼容数据（映射到旧接口）
typedef struct {
    int16_t count;         // 对应 OBD-II 转速 (RPM) 取整
    int8_t  dir;           // 保留（方向）
    float   speed_rpm;     // 对应 OBD-II 车速 (km/h)
    float   angle;         // 对应 OBD-II 节气门位置 (%)
} Encoder_Data_t;

// OBD-II 完整诊断数据
typedef struct {
    float   rpm;              // 发动机转速 (RPM)        - PID 0x0C
    float   speed_kmh;        // 车速 (km/h)             - PID 0x0D
    float   coolant_temp;     // 冷却液温度 (°C)         - PID 0x05
    float   throttle_pct;     // 节气门位置 (%)          - PID 0x11
    float   engine_load;      // 发动机负荷 (%)          - PID 0x04
    int     connected;        // ECU 是否响应（1=连接）
} Obd2_Data_t;

// 驱动状态
typedef enum {
    OBD2_STATE_IDLE,       // 未初始化
    OBD2_STATE_RUNNING,    // 运行中
    OBD2_STATE_ERROR       // 错误
} obd2_state_t;

// OBD-II 驱动管理器
typedef struct {
    int sockfd;                  // UDP socket

    pthread_t rx_tid;            // 接收线程
    pthread_t tx_tid;            // 发送线程（轮询 PID）
    volatile bool running;       // 运行标志

    Encoder_Data_t encoder;      // 编码器兼容数据
    Obd2_Data_t    obd2;         // 完整 OBD-II 数据
    pthread_mutex_t data_mutex;  // 数据互斥锁

    uint32_t rx_count;           // 接收计数
    uint32_t tx_count;           // 发送计数
    uint32_t err_count;          // 错误计数
    obd2_state_t state;          // 当前状态
} obd2_drv_t;

// ========== 驱动接口 ==========
int  obd2_drv_init(obd2_drv_t *drv);                              // 初始化（UDP 127.0.0.1:3500）
int  obd2_drv_get_encoder(obd2_drv_t *drv, Encoder_Data_t *enc);  // 获取编码器数据
int  obd2_drv_get_obd2(obd2_drv_t *drv, Obd2_Data_t *data);      // 获取完整 OBD-II 数据
void obd2_drv_deinit(obd2_drv_t *drv);                             // 反初始化

#endif