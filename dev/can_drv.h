#ifndef _CAN_DRV_H_
#define _CAN_DRV_H_

#include <stdint.h>
#include <pthread.h> 
#include <stdbool.h> 

/* ========== 编码器数据 ========== */
typedef struct {
    int16_t count;
    int8_t  dir;
    float   speed_rpm;
    float   angle;
} Encoder_Data_t;

/* ========== 驱动状态机 ========== */
typedef enum {
    CAN_STATE_IDLE,
    CAN_STATE_INIT,
    CAN_STATE_RUNNING,
    CAN_STATE_ERROR
} can_drv_state_t;

/* ========== CAN 驱动对象 ========== */
typedef struct {
    /* 底层 socket */
    int sockfd;

    /* 接收线程 */
    pthread_t rx_tid;
    volatile bool running;

    /* 编码器数据 + 保护锁 */
    Encoder_Data_t encoder;
    pthread_mutex_t enc_mutex;

    /* 统计信息 */
    uint32_t rx_count;
    uint32_t tx_count;
    uint32_t err_count;

    /* 状态 */
    can_drv_state_t state;
} can_drv_t;

/* ========== API ========== */
int can_drv_init(can_drv_t *drv);
int can_drv_get_encoder(can_drv_t *drv, Encoder_Data_t *enc);
int can_drv_send(can_drv_t *drv, uint32_t id, const uint8_t *data, uint8_t len);
void can_drv_deinit(can_drv_t *drv);


#endif