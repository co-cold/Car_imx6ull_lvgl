#ifndef _MPU6050_DRV_H_
#define _MPU6050_DRV_H_

/*
 * MPU6050 六轴传感器驱动
 *
 * 功能：读取加速度（3轴）和角速度（3轴）原始数据。
 *       支持两种读取模式（自动选择）：
 *         1. I2C 字符设备：open/read/close            — /dev/I2C1_mpu6050
 *         2. IIO sysfs 属性文件：open/read/close       — /sys/bus/iio/.../in_accel_x_raw
 *       互补滤波计算俯仰/翻滚角度。
 *       实时检测驾驶行为：急刹、急加速、转向过猛。
 *
 * 数据采集率：约 100Hz（IIO sysfs 模式）或硬件中断驱动
 */

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#include <stddef.h>

// 互补滤波系数（越大=越信任加速度计）
#define MPU6050_COMPLEMENTARY_ALPHA  0.98f

// 驾驶行为类型
typedef enum {
    MPU6050_BEHAVIOR_NONE = 0,           // 正常行驶
    MPU6050_BEHAVIOR_EMERGENCY_BRAKE,    // 急刹（ax < -4.0 m/s²）
    MPU6050_BEHAVIOR_RAPID_ACCEL,        // 急加速（ax > 3.0 m/s²）
    MPU6050_BEHAVIOR_SHARP_TURN,         // 转向过猛（|gz| > 100 dps）
    MPU6050_BEHAVIOR_COUNT
} mpu6050_behavior_t;

// 传感器处理后的输出数据
typedef struct {
    int16_t  ax_raw, ay_raw, az_raw;     // 加速度原始值（LSB）
    int16_t  gx_raw, gy_raw, gz_raw;     // 角速度原始值（LSB）

    float    ax_mss, ay_mss, az_mss;     // 加速度（m/s²）
    float    gx_dps, gy_dps, gz_dps;     // 角速度（°/s）

    float    pitch_deg;                  // 俯仰角（°）
    float    roll_deg;                   // 翻滚角（°）

    float    acc_mag;                    // 加速度幅值
    float    gyro_mag;                   // 角速度幅值

    mpu6050_behavior_t behavior;         // 当前驾驶行为
} Mpu6050_Data_t;

// 传感器原始值（6通道）
typedef struct {
    int16_t  ax, ay, az;
    int16_t  gx, gy, gz;
} mpu6050_raw_t;

// 驱动状态
typedef enum {
    MPU6050_STATE_IDLE,      // 未初始化
    MPU6050_STATE_RUNNING,   // 运行中
    MPU6050_STATE_ERROR      // 错误
} mpu6050_state_t;

// MPU6050 驱动管理器
typedef struct {
    char             device[64];         // 设备路径

    int              fd;                 // 设备文件描述符（I2C 模式）或 -1（sysfs 模式）
    int              dev_num;            // IIO 设备编号（>=0 走 sysfs 读，<0 走 I2C）

    pthread_t        rx_tid;             // 数据采集线程
    volatile bool    running;            // 运行标志

    Mpu6050_Data_t   data;              // 最新数据（线程安全）
    pthread_mutex_t  data_mutex;         // 数据互斥锁

    mpu6050_behavior_t last_behavior;   // 上一次行为状态
    uint32_t           behavior_start_ms; // 行为开始时间（用于去抖）

    // 校准偏移（零偏补偿）
    float            ax_bias, ay_bias, az_bias;
    float            gx_bias, gy_bias, gz_bias;

    // 硬件标定参数
    int16_t          ax_offset, ay_offset, az_offset;
    int16_t          gx_offset, gy_offset, gz_offset;

    uint16_t         accel_sensitivity;  // 加速度灵敏度（LSB/g）
    float            gyro_sensitivity;   // 陀螺仪灵敏度（LSB/°/s）

    float            smooth_pitch;       // 互补滤波俯仰角
    float            smooth_roll;        // 互补滤波翻滚角
    uint64_t         last_update_us;     // 上次更新时间（微秒）

    uint32_t         rx_count;           // 成功读取次数
    uint32_t         err_count;          // 错误次数
    mpu6050_state_t  state;              // 当前状态
} mpu6050_drv_t;

// ========== 驱动接口 ==========
int  mpu6050_drv_init(mpu6050_drv_t *drv, const char *device);
int  mpu6050_drv_get_data(mpu6050_drv_t *drv, Mpu6050_Data_t *data);
void mpu6050_drv_deinit(mpu6050_drv_t *drv);

// ========== 自动检测 ==========
int  mpu6050_drv_autodetect(char *device_out, size_t size);      // 自动发现 MPU6050 设备

#endif