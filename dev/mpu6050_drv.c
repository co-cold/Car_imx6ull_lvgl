/*
 * mpu6050_drv.c — MPU6050 六轴传感器驱动实现
 *
 * 支持两种读取模式（自动选择）：
 *   1. I2C 字符设备：open/read/close — /dev/I2C1_mpu6050
 *   2. IIO sysfs   ：open/read/close — /sys/bus/iio/.../in_accel_x_raw
 * 互补滤波 + 驾驶行为检测（急刹/急加速/转向）
 */
#include "mpu6050_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <sys/time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ========== 时间工具 ==========

static uint64_t mpu6050_get_time_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

static uint32_t mpu6050_get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000ULL + tv.tv_usec / 1000);
}

// ========== 原始数据读取（sysfs 逐属性文件） ==========

static int mpu6050_read_raw(mpu6050_drv_t *drv, mpu6050_raw_t *raw)
{
    if (drv->dev_num >= 0) {
        static const char *attrs[6] = {
            "in_accel_x_raw",  "in_accel_y_raw",  "in_accel_z_raw",
            "in_anglvel_x_raw","in_anglvel_y_raw","in_anglvel_z_raw"
        };
        int16_t *dst[6] = { &raw->ax, &raw->ay, &raw->az,
                            &raw->gx, &raw->gy, &raw->gz };
        char path[256], val[32];

        for (int i = 0; i < 6; i++) {
            snprintf(path, sizeof(path),
                "/sys/bus/iio/devices/iio:device%d/%s",
                drv->dev_num, attrs[i]);
            int fd = open(path, O_RDONLY);
            if (fd < 0) { drv->err_count++; return -1; }
            int n = read(fd, val, sizeof(val) - 1);
            close(fd);
            if (n <= 0) { drv->err_count++; return -1; }
            val[n] = '\0';
            *dst[i] = (int16_t)atoi(val);
        }
        return 0;
    }

    /* I2C 字符设备：直接 read 12 字节 */
    int16_t buf[6];
    int ret = read(drv->fd, buf, sizeof(buf));
    if (ret != (int)sizeof(buf)) {
        drv->err_count++;
        return -1;
    }
    raw->ax = buf[0]; raw->ay = buf[1]; raw->az = buf[2];
    raw->gx = buf[3]; raw->gy = buf[4]; raw->gz = buf[5];
    return 0;
}

// ========== 标定和单位换算 ==========

static void mpu6050_apply_calibration(mpu6050_drv_t *drv, mpu6050_raw_t *raw)
{
    raw->ax -= drv->ax_offset;
    raw->ay -= drv->ay_offset;
    raw->az -= drv->az_offset;
    raw->gx -= drv->gx_offset;
    raw->gy -= drv->gy_offset;
    raw->gz -= drv->gz_offset;
}

static void mpu6050_convert_units(mpu6050_drv_t *drv, mpu6050_raw_t *raw, Mpu6050_Data_t *data)
{
    data->ax_mss = (float)raw->ax / (float)drv->accel_sensitivity;
    data->ay_mss = (float)raw->ay / (float)drv->accel_sensitivity;
    data->az_mss = (float)raw->az / (float)drv->accel_sensitivity;

    data->gx_dps = (float)raw->gx / drv->gyro_sensitivity;
    data->gy_dps = (float)raw->gy / drv->gyro_sensitivity;
    data->gz_dps = (float)raw->gz / drv->gyro_sensitivity;

    data->acc_mag  = sqrtf(data->ax_mss * data->ax_mss +
                           data->ay_mss * data->ay_mss +
                           data->az_mss * data->az_mss);
    data->gyro_mag = sqrtf(data->gx_dps * data->gx_dps +
                           data->gy_dps * data->gy_dps +
                           data->gz_dps * data->gz_dps);
}

// ========== 互补滤波（俯仰/翻滚角） ==========

static void mpu6050_complementary_filter(mpu6050_drv_t *drv, Mpu6050_Data_t *data, double dt_s)
{
    float pitch_acc = atan2f(data->ay_mss, sqrtf(data->ax_mss * data->ax_mss + data->az_mss * data->az_mss));
    float roll_acc  = atan2f(-data->ax_mss, data->az_mss);

    pitch_acc = pitch_acc * 180.0f / (float)M_PI;
    roll_acc  = roll_acc  * 180.0f / (float)M_PI;

    if (drv->last_update_us == 0) {
        drv->smooth_pitch = pitch_acc;
        drv->smooth_roll  = roll_acc;
    } else {
        drv->smooth_pitch = MPU6050_COMPLEMENTARY_ALPHA *
            (drv->smooth_pitch + data->gy_dps * (float)dt_s) +
            (1.0f - MPU6050_COMPLEMENTARY_ALPHA) * pitch_acc;

        drv->smooth_roll = MPU6050_COMPLEMENTARY_ALPHA *
            (drv->smooth_roll + data->gx_dps * (float)dt_s) +
            (1.0f - MPU6050_COMPLEMENTARY_ALPHA) * roll_acc;
    }

    data->pitch_deg = drv->smooth_pitch;
    data->roll_deg  = drv->smooth_roll;
}

// ========== 驾驶行为检测（急刹/急加速/转向） ==========

static mpu6050_behavior_t mpu6050_detect_behavior(mpu6050_drv_t *drv, Mpu6050_Data_t *data)
{
    uint32_t now_ms = mpu6050_get_time_ms();

    if (data->ax_mss < -3.0f) {
        if (drv->last_behavior != MPU6050_BEHAVIOR_EMERGENCY_BRAKE) {
            drv->last_behavior    = MPU6050_BEHAVIOR_EMERGENCY_BRAKE;
            drv->behavior_start_ms = now_ms;
        } else if (now_ms - drv->behavior_start_ms >= 200) {
            return MPU6050_BEHAVIOR_EMERGENCY_BRAKE;
        }
    } else if (data->ax_mss > 2.5f) {
        if (drv->last_behavior != MPU6050_BEHAVIOR_RAPID_ACCEL) {
            drv->last_behavior    = MPU6050_BEHAVIOR_RAPID_ACCEL;
            drv->behavior_start_ms = now_ms;
        } else if (now_ms - drv->behavior_start_ms >= 200) {
            return MPU6050_BEHAVIOR_RAPID_ACCEL;
        }
    } else if (fabsf(data->gz_dps) > 30.0f) {
        if (drv->last_behavior != MPU6050_BEHAVIOR_SHARP_TURN) {
            drv->last_behavior    = MPU6050_BEHAVIOR_SHARP_TURN;
            drv->behavior_start_ms = now_ms;
        } else if (now_ms - drv->behavior_start_ms >= 300) {
            return MPU6050_BEHAVIOR_SHARP_TURN;
        }
    } else {
        drv->last_behavior = MPU6050_BEHAVIOR_NONE;
    }

    return MPU6050_BEHAVIOR_NONE;
}

// ========== 传感器采集线程 ==========

static void *mpu6050_rx_thread(void *arg)
{
    mpu6050_drv_t *drv = (mpu6050_drv_t *)arg;

    while (drv->running) {
        mpu6050_raw_t raw;
        if (mpu6050_read_raw(drv, &raw) != 0) {
            usleep(5000);
            continue;
        }

        uint64_t now_us = mpu6050_get_time_us();

        mpu6050_apply_calibration(drv, &raw);

        Mpu6050_Data_t data;
        memset(&data, 0, sizeof(data));

        data.ax_raw = raw.ax;
        data.ay_raw = raw.ay;
        data.az_raw = raw.az;
        data.gx_raw = raw.gx;
        data.gy_raw = raw.gy;
        data.gz_raw = raw.gz;

        mpu6050_convert_units(drv, &raw, &data);

        double dt_s = 0.0;
        if (drv->last_update_us > 0)
            dt_s = (double)(now_us - drv->last_update_us) / 1000000.0;

        mpu6050_complementary_filter(drv, &data, dt_s);
        data.behavior = mpu6050_detect_behavior(drv, &data);

        pthread_mutex_lock(&drv->data_mutex);
        drv->data = data;
        drv->last_update_us = now_us;
        pthread_mutex_unlock(&drv->data_mutex);

        drv->rx_count++;

        usleep(10000);
    }
    return NULL;
}

// ========== 公共 API（初始化、数据读取、反初始化、自动检测） ==========

int mpu6050_drv_init(mpu6050_drv_t *drv, const char *device)
{
    if (!drv || !device) return -1;

    memset(drv, 0, sizeof(*drv));
    strncpy(drv->device, device, sizeof(drv->device) - 1);
    drv->dev_num = -1;
    drv->fd      = -1;

    if (strncmp(device, "/dev/iio:device", 15) == 0) {
        sscanf(device, "/dev/iio:device%d", &drv->dev_num);
        printf("[mpu6050_drv] IIO sysfs mode, iio:device%d\n", drv->dev_num);
    } else {
        drv->fd = open(device, O_RDWR);
        if (drv->fd < 0) {
            fprintf(stderr, "[mpu6050_drv] open %s failed: %s\n",
                    device, strerror(errno));
            return -1;
        }
    }

    drv->ax_offset = 0;
    drv->ay_offset = 0;
    drv->az_offset = 0;
    drv->gx_offset = 0;
    drv->gy_offset = 0;
    drv->gz_offset = 0;

    drv->accel_sensitivity = 16384;
    drv->gyro_sensitivity  = 131.0f;

    pthread_mutex_init(&drv->data_mutex, NULL);
    drv->running = true;
    drv->state   = MPU6050_STATE_RUNNING;

    if (pthread_create(&drv->rx_tid, NULL, mpu6050_rx_thread, drv) != 0) {
        fprintf(stderr, "[mpu6050_drv] pthread_create rx failed\n");
        drv->running = false;
        close(drv->fd);
        return -1;
    }

    printf("[mpu6050_drv] initialized, device=%s\n", device);
    return 0;
}

int mpu6050_drv_get_data(mpu6050_drv_t *drv, Mpu6050_Data_t *data)
{
    if (!drv || !data) return -1;
    pthread_mutex_lock(&drv->data_mutex);
    *data = drv->data;
    pthread_mutex_unlock(&drv->data_mutex);
    return 0;
}

void mpu6050_drv_deinit(mpu6050_drv_t *drv)
{
    if (!drv) return;
    drv->running = false;
    if (drv->rx_tid) {
        pthread_join(drv->rx_tid, NULL);
        drv->rx_tid = 0;
    }
    if (drv->fd >= 0) {
        close(drv->fd);
        drv->fd = -1;
    }
    pthread_mutex_destroy(&drv->data_mutex);
    printf("[mpu6050_drv] deinitialized, rx=%u err=%u\n", drv->rx_count, drv->err_count);
}

int mpu6050_drv_autodetect(char *device_out, size_t size)
{
    if (!device_out || size == 0) return -1;

    /* 1. 野火 I2C 字符设备 */
    if (access("/dev/I2C1_mpu6050", F_OK) == 0) {
        snprintf(device_out, size, "/dev/I2C1_mpu6050");
        printf("[mpu6050_drv] autodetect: found /dev/I2C1_mpu6050\n");
        return 0;
    }

    /* 2. IIO 子系统设备 */
    for (int i = 0; i < 10; i++) {
        char path[128];
        snprintf(path, sizeof(path),
                 "/sys/bus/iio/devices/iio:device%d/name", i);
        FILE *f = fopen(path, "r");
        if (!f) continue;
        char buf[64];
        if (!fgets(buf, sizeof(buf), f)) { fclose(f); continue; }
        fclose(f);
        if (strstr(buf, "mpu") || strstr(buf, "MPU")) {
            snprintf(device_out, size, "/dev/iio:device%d", i);
            printf("[mpu6050_drv] autodetect: found MPU6050 at iio:device%d\n", i);
            return 0;
        }
    }

    fprintf(stderr, "[mpu6050_drv] autodetect: no MPU6050 found\n");
    return -1;
}