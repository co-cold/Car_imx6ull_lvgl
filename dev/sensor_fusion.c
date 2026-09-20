/*
 * sensor_fusion.c — 多源传感器融合实现
 *
 * 数据流：D-Bus 信号 → IPC 回调 → 融合模块 → Fusion_Data_t
 *
 * 核心算法：
 *   - 速度融合：v_fused = 0.7 * v_uwb + 0.3 * v_imu
 *   - 加速度积分：v_imu += ax * dt
 *   - 碰撞风险：距离 < 50cm 且接近速度 > 50cm/s
 *   - 传感器有效性：对端 D-Bus 信号驱动
 */
#include "sensor_fusion.h"
#include "ipc_uwb.h"
#include "ipc_mpu6050.h"
#include "ipc_obd2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

// ========== 融合参数 ==========

#define FUSION_UWB_WEIGHT    0.7f    // UWB 速度权重
#define FUSION_IMU_WEIGHT    0.3f    // IMU 速度权重
#define COLLISION_DIST_CM    50.0f   // 碰撞预警距离（厘米）
#define COLLISION_SPEED_CMS  50.0f   // 碰撞预警接近速度（cm/s）

/* 融合内部状态 */
static struct {
    Fusion_Data_t    data;
    pthread_mutex_t  mutex;

    /* UWB 速度计算 */
    float            uwb_last_dist_mm;
    uint64_t         uwb_last_us;

    /* MPU6050 速度积分 */
    float            imu_vel_cms;
    uint64_t         imu_last_us;

    /* 行为计数器 */
    mpu6050_behavior_t last_behavior;
    int              brake_cnt;
    int              accel_cnt;
    int              turn_cnt;
    float            accel_peak;

    /* 倒挡 */
    bool             reverse_gear;

    /* 运行标志 */
    bool             uwb_valid;
    bool             mpu6050_valid;
    bool             obd2_valid;
} g_fusion;

static uint32_t get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000ULL + tv.tv_usec / 1000);
}

// ========== D-Bus 回调（接收 UWB / MPU6050 / OBD2 数据） ==========

static void on_uwb_data(const Uwb_Data_t *uwb, void *user_data)
{
    (void)user_data;

    pthread_mutex_lock(&g_fusion.mutex);

    g_fusion.uwb_valid = true;
    g_fusion.data.uwb_valid = true;

    g_fusion.data.distance_raw_mm       = (float)uwb->distance_mm;
    g_fusion.data.distance_cm           = (float)uwb->distance_filtered_mm / 10.0f;
    g_fusion.data.distance_change_rate_cms = uwb->distance_change_rate / 10.0f;

    /* UWB 速度分量 */
    float uwb_speed_cms = uwb->distance_change_rate / 10.0f;

    uint64_t now_us = (uint64_t)uwb->timestamp_ms * 1000ULL;
    if (g_fusion.uwb_last_us > 0 && now_us > g_fusion.uwb_last_us) {
        /* 使用 UWB 内置速率 */
    }
    g_fusion.uwb_last_us = now_us;

    /* 融合速度：UWB(0.7) + IMU(0.3) */
    float fused = uwb_speed_cms * FUSION_UWB_WEIGHT +
                  g_fusion.imu_vel_cms * FUSION_IMU_WEIGHT;
    g_fusion.data.speed_fused_cms = fused;
    g_fusion.data.speed_fused_kmh = fused * 0.036f;

    /* 碰撞风险判定 */
    g_fusion.data.collision_risk =
        (g_fusion.data.distance_cm < COLLISION_DIST_CM) &&
        (fabsf(fused) > COLLISION_SPEED_CMS);

    g_fusion.data.last_update_ms = get_time_ms();
    pthread_mutex_unlock(&g_fusion.mutex);
}

static void on_mpu6050_data(const Mpu6050_Data_t *imu, void *user_data)
{
    (void)user_data;

    pthread_mutex_lock(&g_fusion.mutex);

    g_fusion.mpu6050_valid = true;
    g_fusion.data.mpu6050_valid = true;

    g_fusion.data.accel_mss  = imu->acc_mag;
    g_fusion.data.gyro_dps   = imu->gyro_mag;
    g_fusion.data.pitch_deg  = imu->pitch_deg;
    g_fusion.data.roll_deg   = imu->roll_deg;

    /* IMU 速度积分：沿 Y 轴（前进方向）加速度积分 */
    uint64_t us_now = (uint64_t)get_time_ms() * 1000ULL;
    if (g_fusion.imu_last_us > 0) {
        double dt = (double)(us_now - g_fusion.imu_last_us) / 1000000.0;
        /* 用 Y 轴加速度（前进方向）积分速度 */
        g_fusion.imu_vel_cms += imu->ay_mss * (float)dt * 100.0f;
    }
    g_fusion.imu_last_us = us_now;

    /* 加速度峰值 */
    if (imu->acc_mag > g_fusion.accel_peak)
        g_fusion.accel_peak = imu->acc_mag;
    g_fusion.data.accel_peak = g_fusion.accel_peak;

    /* 行为计数 */
    if (imu->behavior != MPU6050_BEHAVIOR_NONE &&
        imu->behavior != g_fusion.last_behavior) {
        switch (imu->behavior) {
            case MPU6050_BEHAVIOR_EMERGENCY_BRAKE: g_fusion.brake_cnt++; break;
            case MPU6050_BEHAVIOR_RAPID_ACCEL:      g_fusion.accel_cnt++;  break;
            case MPU6050_BEHAVIOR_SHARP_TURN:       g_fusion.turn_cnt++;   break;
            default: break;
        }
    }
    g_fusion.last_behavior = imu->behavior;
    g_fusion.data.behavior     = imu->behavior;
    g_fusion.data.brake_count  = g_fusion.brake_cnt;
    g_fusion.data.accel_count  = g_fusion.accel_cnt;
    g_fusion.data.turn_count   = g_fusion.turn_cnt;

    g_fusion.data.last_update_ms = get_time_ms();
    pthread_mutex_unlock(&g_fusion.mutex);
}

static void on_obd2_data(const Encoder_Data_t *obd2, void *user_data)
{
    (void)user_data;

    pthread_mutex_lock(&g_fusion.mutex);

    g_fusion.obd2_valid = true;
    g_fusion.data.obd2_valid = true;

    g_fusion.data.vehicle_speed_kmh = obd2->speed_rpm;  /* speed_rpm 实际存 km/h */
    g_fusion.data.engine_rpm        = (float)obd2->count;
    g_fusion.data.throttle_pct      = obd2->angle;

    g_fusion.data.last_update_ms = get_time_ms();
    pthread_mutex_unlock(&g_fusion.mutex);
}

// ========== 公共 API ==========

int sensor_fusion_init(const char *bus_address)
{
    memset(&g_fusion, 0, sizeof(g_fusion));
    pthread_mutex_init(&g_fusion.mutex, NULL);

    if (ipc_uwb_init(bus_address, on_uwb_data, NULL) != 0) {
        fprintf(stderr, "[fusion] ipc_uwb init failed\n");
    }

    if (ipc_mpu6050_init(bus_address, on_mpu6050_data, NULL) != 0) {
        fprintf(stderr, "[fusion] ipc_mpu6050 init failed\n");
    }

    if (ipc_obd2_init(bus_address, on_obd2_data, NULL) != 0) {
        fprintf(stderr, "[fusion] ipc_obd2 init failed\n");
    }

    printf("[fusion] initialized, bus=%s\n", bus_address);
    return 0;
}

void sensor_fusion_deinit(void)
{
    ipc_uwb_deinit();
    ipc_mpu6050_deinit();
    ipc_obd2_deinit();
    pthread_mutex_destroy(&g_fusion.mutex);
    printf("[fusion] deinitialized\n");
}

/* ── 获取融合数据 ──────────────────────────────────── */
int sensor_fusion_get_data(Fusion_Data_t *data)
{
    if (!data) return -1;
    pthread_mutex_lock(&g_fusion.mutex);
    *data = g_fusion.data;
    pthread_mutex_unlock(&g_fusion.mutex);
    return 0;
}

/* ── 设置倒挡 ──────────────────────────────────────── */
void sensor_fusion_set_reverse_gear(bool reverse)
{
    pthread_mutex_lock(&g_fusion.mutex);
    g_fusion.reverse_gear = reverse;
    g_fusion.data.is_reverse_gear = reverse;
    pthread_mutex_unlock(&g_fusion.mutex);
}

/* ── 重置统计 ──────────────────────────────────────── */
void sensor_fusion_reset_stats(void)
{
    pthread_mutex_lock(&g_fusion.mutex);
    g_fusion.brake_cnt   = 0;
    g_fusion.accel_cnt   = 0;
    g_fusion.turn_cnt    = 0;
    g_fusion.accel_peak  = 0.0f;
    g_fusion.imu_vel_cms = 0.0f;
    g_fusion.imu_last_us = 0;

    g_fusion.data.brake_count = 0;
    g_fusion.data.accel_count = 0;
    g_fusion.data.turn_count  = 0;
    g_fusion.data.accel_peak  = 0.0f;
    pthread_mutex_unlock(&g_fusion.mutex);
    printf("[fusion] stats reset\n");
}