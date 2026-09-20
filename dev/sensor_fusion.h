#ifndef _SENSOR_FUSION_H_
#define _SENSOR_FUSION_H_

/*
 * 多源传感器融合模块
 *
 * 功能：将 UWB 距离、MPU6050 姿态、OBD-II 车速三个独立数据源
 *       融合为统一输出结构 Fusion_Data_t。
 *
 * 核心算法：
 *   - 速度融合：v_fused = 0.7 * v_uwb + 0.3 * v_imu
 *   - 碰撞风险：距离<50cm + 接近速度>50cm/s → 高风险
 *   - 驾驶行为：急刹/急加速/转向计数统计
 *   - 传感器在线状态：自动检测各数据源是否存活
 *
 * 数据流：D-Bus 信号 → fusion 回调 → 更新内部状态 → UI 100ms 轮询
 */

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "mpu6050_drv.h"

/* 融合后的统一数据结构 */
typedef struct {
    /* —— 距离 —— */
    float    distance_cm;                  // UWB 滤波后距离（厘米）
    float    distance_raw_mm;              // UWB 原始距离（毫米）
    float    distance_change_rate_cms;     // UWB 变化率（cm/s，正值=远离）

    /* —— 倒车速度（UWB + MPU6050 加权融合） —— */
    float    speed_fused_cms;              // 融合速度（cm/s）
    float    speed_fused_kmh;              // 融合速度（km/h）

    /* —— MPU6050 姿态 —— */
    float    accel_mss;                    // 加速度幅值（m/s²）
    float    gyro_dps;                     // 角速度幅值（°/s）
    float    pitch_deg;                    // 俯仰角（°）
    float    roll_deg;                     // 翻滚角（°）

    /* —— 驾驶行为 —— */
    mpu6050_behavior_t behavior;          // 当前驾驶行为
    int      brake_count;                  // 急刹次数
    int      accel_count;                  // 急加速次数
    int      turn_count;                   // 转向过猛次数
    float    accel_peak;                   // 本次会话加速度峰值

    /* —— OBD-II —— */
    float    vehicle_speed_kmh;            // OBD 车速（km/h）
    float    engine_rpm;                   // 发动机转速（RPM）
    float    throttle_pct;                 // 节气门位置（%）

    /* —— 碰撞风险 —— */
    bool     collision_risk;               // true = 有碰撞风险

    /* —— 挡位 —— */
    bool     is_reverse_gear;              // true = 倒挡

    /* —— 传感器在线状态 —— */
    bool     uwb_valid;                    // UWB 传感器在线
    bool     mpu6050_valid;                // MPU6050 传感器在线
    bool     obd2_valid;                   // OBD-II 在线
    uint32_t last_update_ms;               // 最后更新时间
} Fusion_Data_t;

// 融合数据更新回调
typedef void (*fusion_data_cb_t)(const Fusion_Data_t *data, void *user_data);

// ========== 融合接口 ==========
int  sensor_fusion_init(const char *bus_address);       // 初始化（连接 D-Bus）
void sensor_fusion_deinit(void);                         // 反初始化

int  sensor_fusion_get_data(Fusion_Data_t *data);       // 获取融合数据（线程安全）

void sensor_fusion_set_reverse_gear(bool reverse);      // 设置倒挡状态

void sensor_fusion_reset_stats(void);                    // 重置驾驶行为统计

#endif