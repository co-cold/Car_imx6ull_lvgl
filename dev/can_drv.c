#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

#include "can_drv.h"

#define CAN_IFNAME     "can0"
#define CAN_ID_ENCODER 0x100
#define CAN_SEND_ID    0x123

/**
 * 解析编码器数据帧
 */
static int parse_encoder(const uint8_t data[8], Encoder_Data_t *enc)
{
    if (!enc) return -1;

    enc->count = (int16_t)(data[0] | (data[1] << 8));
    enc->dir   = (int8_t)data[2];

    int16_t speed_i = (int16_t)(data[3] | (data[4] << 8));
    int16_t angle_i = (int16_t)(data[5] | (data[6] << 8));

    enc->speed_rpm = speed_i / 10.0f;
    enc->angle     = angle_i / 10.0f;

    return 0;
}

/**
 * 初始化 CAN 驱动：创建 socket、绑定接口、设置过滤器
 */
static int socketcan_init(can_drv_t *drv)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* 1. 创建 CAN 原始套接字 */
    drv->sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (drv->sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* 2. 指定 CAN 接口名（can0） */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, CAN_IFNAME, IFNAMSIZ);

    /* 3. 获取 can0 的接口索引 */
    if (ioctl(drv->sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(drv->sockfd);
        return -1;
    }

    /* 4. 绑定 CAN 接口 */
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(drv->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(drv->sockfd);
        return -1;
    }

    /* 5. 设置 CAN 接收过滤器（只接收指定 ID） */
    struct can_filter filter = {
        .can_id = CAN_ID_ENCODER,
        .can_mask = CAN_SFF_MASK
    };

    /* 应用过滤器到 socket */
    if (setsockopt(drv->sockfd, SOL_CAN_RAW, CAN_RAW_FILTER,
                   &filter, sizeof(filter)) < 0) {
        perror("setsockopt filter");
        close(drv->sockfd);
        return -1;
    }

    return 0;
}

/**
 * 接收线程函数：不断读取 CAN 消息并解析
 */
static void *rx_thread(void *arg)
{
    can_drv_t *drv = (can_drv_t *)arg;
    struct can_frame frame;

    while (drv->running) {
        int nbytes = read(drv->sockfd, &frame, sizeof(frame));
        if (nbytes < 0) {
            if (errno == EINTR) continue;
            break;
        }

        if (frame.can_id == CAN_ID_ENCODER) {
            Encoder_Data_t enc;
            if (parse_encoder(frame.data, &enc) == 0) {
                pthread_mutex_lock(&drv->enc_mutex);
                drv->encoder = enc;
                pthread_mutex_unlock(&drv->enc_mutex);
                drv->rx_count++;
            }
        }
        /* else if 其他 ID ... */
    }
    return NULL;
}

/* ========== 公开 API ========== */
int can_drv_init(can_drv_t *drv)
{
    if (!drv) return -1;

    memset(drv, 0, sizeof(*drv));
    pthread_mutex_init(&drv->enc_mutex, NULL);

    if (socketcan_init(drv) != 0) {
        drv->state = CAN_STATE_ERROR;
        return -1;
    }

    drv->running = true;
    drv->state = CAN_STATE_RUNNING;

    if (pthread_create(&drv->rx_tid, NULL, rx_thread, drv) != 0) {
        perror("pthread_create");
        close(drv->sockfd);
        drv->state = CAN_STATE_ERROR;
        return -1;
    }

    return 0;
}

int can_drv_get_encoder(can_drv_t *drv, Encoder_Data_t *enc)
{
    if (!drv || !enc) return -1;

    pthread_mutex_lock(&drv->enc_mutex);
    *enc = drv->encoder;
    pthread_mutex_unlock(&drv->enc_mutex);

    return 0;
}

int can_drv_send(can_drv_t *drv, uint32_t id,
                 const uint8_t *data, uint8_t len)
{
    struct can_frame frame = {0};

    frame.can_id  = id;
    frame.can_dlc = len > 8 ? 8 : len;
    memcpy(frame.data, data, frame.can_dlc);

    if (write(drv->sockfd, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("write");
        drv->err_count++;
        return -1;
    }
    drv->tx_count++;
    return 0;
}

void can_drv_deinit(can_drv_t *drv)
{
    if (!drv) return;

    drv->running = false;

    if (drv->sockfd >= 0) {
        close(drv->sockfd);
        drv->sockfd = -1;
    }

    pthread_join(drv->rx_tid, NULL);
    pthread_mutex_destroy(&drv->enc_mutex);

    drv->state = CAN_STATE_IDLE;
}


