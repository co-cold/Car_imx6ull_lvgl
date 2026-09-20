#ifndef _UWB_DRIVER_H_
#define _UWB_DRIVER_H_

/*
 * UWB 串口驱动（DW1000 / NodeMCU-BU01）
 *
 * 功能：串口接收 UWB 基站 "mc" 帧数据，解析距离/RSSI/序列号。
 *       内置中值滤波 + 滑动平均去抖，计算距离变化率（速度）。
 *       支持自动检测：扫描 /dev/ttyUSB* 和 /dev/ttyACM*，
 *       通过识别 "mc" 数据帧确认 UWB 设备。
 *       支持运行时重连（热插拔）。
 *
 * 数据采集率：约 50Hz（460800bps 下每帧 ~27字节）
 */

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

// 滤波窗口大小
#define UWB_MEDIAN_WINDOW   5    // 中值滤波窗口
#define UWB_AVERAGE_WINDOW  10   // 滑动平均窗口

// 单帧 UWB 数据
typedef struct {
    uint32_t distance_mm;            // 原始距离（毫米）
    uint32_t distance_filtered_mm;   // 滤波/平滑后距离
    float    distance_change_rate;   // 距离变化率（mm/s，正值=远离）
    int      rssi;                   // 信号强度（dB）
    uint32_t raw_timestamp;          // UWB 设备时间戳
    uint16_t seq;                    // 帧序号
    uint32_t timestamp_ms;           // 本地时间戳（毫秒）
} Uwb_Data_t;

// 驱动状态
typedef enum {
    UWB_STATE_IDLE,      // 未初始化
    UWB_STATE_RUNNING,   // 运行中（接收数据）
    UWB_STATE_ERROR      // 错误（设备断连等）
} uwb_state_t;

// UWB 驱动管理器
typedef struct {
    char     device[64];             // 设备路径（如 /dev/ttyACM0）
    int      fd;                     // 串口文件描述符（-1 表示未打开）
    int      baudrate;               // 波特率

    pthread_t        rx_tid;         // 接收线程
    volatile bool    running;        // 运行标志

    Uwb_Data_t       data;           // 最新数据（线程安全）
    pthread_mutex_t  data_mutex;     // 数据互斥锁

    uint32_t         rx_count;       // 接收帧计数
    uint32_t         err_count;      // 错误帧计数
    uwb_state_t      state;          // 当前状态

    // 中值滤波缓冲区
    uint32_t         median_buf[UWB_MEDIAN_WINDOW];
    int              median_idx;
    int              median_count;

    // 滑动平均缓冲区
    uint32_t         avg_buf[UWB_AVERAGE_WINDOW];
    int              avg_idx;
    int              avg_count;
    uint32_t         avg_sum;

    // 速度计算
    uint32_t         last_distance_mm;     // 上一帧距离
    uint64_t         last_timestamp_us;    // 上一帧时间戳（微秒）
} uwb_drv_t;

// ========== 驱动接口 ==========
int         uwb_drv_init(uwb_drv_t *drv, const char *device, int baudrate);
int         uwb_drv_get_data(uwb_drv_t *drv, Uwb_Data_t *data);
void        uwb_drv_deinit(uwb_drv_t *drv);

// ========== 热插拔支持 ==========
uwb_state_t uwb_drv_get_state(uwb_drv_t *drv);      // 获取驱动状态
int         uwb_drv_reconnect(uwb_drv_t *drv, const char *device, int baudrate);  // 重连设备

// ========== 自动检测 ==========
int         uwb_drv_autodetect(char *device_out, size_t size, int baudrate);      // 自动发现 UWB 设备

#endif