#ifndef _MPU6050_DRV_H_
#define _MPU6050_DRV_H_

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#define MPU6050_COMPLEMENTARY_ALPHA  0.98f

typedef enum {
    MPU6050_BEHAVIOR_NONE = 0,
    MPU6050_BEHAVIOR_EMERGENCY_BRAKE,
    MPU6050_BEHAVIOR_RAPID_ACCEL,
    MPU6050_BEHAVIOR_SHARP_TURN,
    MPU6050_BEHAVIOR_COUNT
} mpu6050_behavior_t;

typedef struct {
    int16_t  ax_raw, ay_raw, az_raw;
    int16_t  gx_raw, gy_raw, gz_raw;

    float    ax_mss, ay_mss, az_mss;
    float    gx_dps, gy_dps, gz_dps;

    float    pitch_deg;
    float    roll_deg;

    float    acc_mag;
    float    gyro_mag;

    mpu6050_behavior_t behavior;
} Mpu6050_Data_t;

typedef struct {
    int16_t  ax, ay, az;
    int16_t  gx, gy, gz;
} mpu6050_raw_t;

typedef enum {
    MPU6050_STATE_IDLE,
    MPU6050_STATE_RUNNING,
    MPU6050_STATE_ERROR
} mpu6050_state_t;

typedef struct {
    char             device[64];

    int              fd;

    pthread_t        rx_tid;
    volatile bool    running;

    Mpu6050_Data_t   data;
    pthread_mutex_t  data_mutex;

    mpu6050_behavior_t last_behavior;
    uint32_t           behavior_start_ms;

    float            ax_bias, ay_bias, az_bias;
    float            gx_bias, gy_bias, gz_bias;

    int16_t          ax_offset, ay_offset, az_offset;
    int16_t          gx_offset, gy_offset, gz_offset;

    uint16_t         accel_sensitivity;
    float            gyro_sensitivity;

    float            smooth_pitch;
    float            smooth_roll;
    uint64_t         last_update_us;

    uint32_t         rx_count;
    uint32_t         err_count;
    mpu6050_state_t  state;
} mpu6050_drv_t;

int  mpu6050_drv_init(mpu6050_drv_t *drv, const char *device);
int  mpu6050_drv_get_data(mpu6050_drv_t *drv, Mpu6050_Data_t *data);
void mpu6050_drv_deinit(mpu6050_drv_t *drv);

#endif