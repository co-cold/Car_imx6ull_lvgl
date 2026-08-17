#ifndef _OBD2_DRV_H_
#define _OBD2_DRV_H_

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

/* 保持 Encoder_Data_t 兼容原有 IPC/D-Bus 接口 */
typedef struct {
    int16_t count;         /* 对应 OBD-II 转速 (RPM) 取整 */
    int8_t  dir;           /* 保留 */
    float   speed_rpm;     /* 对应 OBD-II 车速 (km/h) */
    float   angle;         /* 对应 OBD-II 节气门位置 (%) */
} Encoder_Data_t;

/* OBD-II 完整数据 */
typedef struct {
    float   rpm;              /* 发动机转速 (RPM)        - PID 0x0C */
    float   speed_kmh;        /* 车速 (km/h)             - PID 0x0D */
    float   coolant_temp;     /* 冷却液温度 (°C)         - PID 0x05 */
    float   throttle_pct;     /* 节气门位置 (%)          - PID 0x11 */
    float   engine_load;      /* 发动机负荷 (%)          - PID 0x04 */
    int     connected;        /* ECU 是否响应 */
} Obd2_Data_t;

typedef enum {
    OBD2_STATE_IDLE,
    OBD2_STATE_RUNNING,
    OBD2_STATE_ERROR
} obd2_state_t;

typedef struct {
    int sockfd;

    pthread_t rx_tid;          /* 接收线程 */
    pthread_t tx_tid;          /* 发送线程 (轮询 PID) */
    volatile bool running;

    Encoder_Data_t encoder;
    Obd2_Data_t    obd2;
    pthread_mutex_t data_mutex;

    uint32_t rx_count;
    uint32_t tx_count;
    uint32_t err_count;
    obd2_state_t state;
} obd2_drv_t;

int  obd2_drv_init(obd2_drv_t *drv);
int  obd2_drv_get_encoder(obd2_drv_t *drv, Encoder_Data_t *enc);
int  obd2_drv_get_obd2(obd2_drv_t *drv, Obd2_Data_t *data);
void obd2_drv_deinit(obd2_drv_t *drv);

#endif