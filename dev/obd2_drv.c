/*
 * obd2_drv.c — OBD-II 诊断驱动实现（SocketCAN）
 *
 * 通过 CAN 总线对 OBD-II ECU 轮询 PID 数据：
 *   PID 0x0C — 发动机转速 (RPM)
 *   PID 0x0D — 车速 (km/h)
 *   PID 0x05 — 冷却液温度 (°C)
 *   PID 0x11 — 节气门位置 (%)
 *   PID 0x04 — 发动机负荷 (%)
 *
 * 双线程设计：
 *   - TX 线程：每 50ms 轮询一个 PID
 *   - RX 线程：接收并解析 ECU 响应
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>

#include "obd2_drv.h"

// ========== CAN 总线配置 ==========

#define CAN_IFNAME           "can0"
#define OBD2_REQUEST_ID      0x7DF
#define OBD2_RESPONSE_ID_MIN 0x7E8
#define OBD2_RESPONSE_ID_MAX 0x7EF

#define PID_ENGINE_RPM       0x0C
#define PID_VEHICLE_SPEED    0x0D
#define PID_COOLANT_TEMP     0x05
#define PID_THROTTLE_POS     0x11
#define PID_ENGINE_LOAD      0x04

#define PID_COUNT            5
#define REQUEST_INTERVAL_MS  50
#define RESPONSE_TIMEOUT_MS  200

#define OBD2_SERVICE_SHOW_DATA 0x01

// ========== SocketCAN + PID 请求 ==========

static const uint8_t pid_list[PID_COUNT] = {
    PID_ENGINE_RPM,
    PID_VEHICLE_SPEED,
    PID_COOLANT_TEMP,
    PID_THROTTLE_POS,
    PID_ENGINE_LOAD,
};

static int socketcan_init(obd2_drv_t *drv)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    drv->sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (drv->sockfd < 0) {
        perror("[obd2] socket");
        return -1;
    }

    struct timeval tv = {
        .tv_sec  = 0,
        .tv_usec = RESPONSE_TIMEOUT_MS * 1000,
    };
    setsockopt(drv->sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, CAN_IFNAME, IFNAMSIZ);

    if (ioctl(drv->sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("[obd2] ioctl");
        close(drv->sockfd);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(drv->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[obd2] bind");
        close(drv->sockfd);
        return -1;
    }

    /* 只接收 OBD-II 响应帧 (0x7E8~0x7EF) */
    struct can_filter filters[1] = {
        { .can_id = OBD2_RESPONSE_ID_MIN, .can_mask = 0x7F0 },
    };

    if (setsockopt(drv->sockfd, SOL_CAN_RAW, CAN_RAW_FILTER,
                   filters, sizeof(filters)) < 0) {
        perror("[obd2] setsockopt filter");
        close(drv->sockfd);
        return -1;
    }

    return 0;
}

static void send_pid_request(obd2_drv_t *drv, uint8_t pid)
{
    struct can_frame frame = {0};
    frame.can_id  = OBD2_REQUEST_ID;
    frame.can_dlc = 8;
    frame.data[0] = 0x02;                    /* PCI: 单帧, 2 字节数据 */
    frame.data[1] = OBD2_SERVICE_SHOW_DATA;  /* Service 01 */
    frame.data[2] = pid;
    /* data[3..7] = 0xAA (填充) */

    if (write(drv->sockfd, &frame, sizeof(frame)) != sizeof(frame)) {
        drv->err_count++;
    } else {
        drv->tx_count++;
    }
}

static int parse_obd2_response(const struct can_frame *frame, Obd2_Data_t *obd2)
{
    uint8_t num_bytes = frame->data[0] & 0x0F;
    if (num_bytes < 2) return -1;

    uint8_t service = frame->data[1];
    if (service != 0x41) return -1;   /* 不是 Service 01 响应 */

    uint8_t pid = frame->data[2];
    uint8_t A   = frame->data[3];
    uint8_t B   = frame->data[4];

    switch (pid) {
    case PID_ENGINE_RPM:
        obd2->rpm = ((A * 256) + B) / 4.0f;
        break;
    case PID_VEHICLE_SPEED:
        obd2->speed_kmh = A;
        break;
    case PID_COOLANT_TEMP:
        obd2->coolant_temp = A - 40;
        break;
    case PID_THROTTLE_POS:
        obd2->throttle_pct = A * 100.0f / 255.0f;
        break;
    case PID_ENGINE_LOAD:
        obd2->engine_load = A * 100.0f / 255.0f;
        break;
    default:
        return -1;
    }

    return 0;
}

// ========== TX / RX 线程 ==========

static void *tx_thread(void *arg)
{
    obd2_drv_t *drv = (obd2_drv_t *)arg;
    int idx = 0;

    while (drv->running) {
        send_pid_request(drv, pid_list[idx]);
        idx = (idx + 1) % PID_COUNT;
        usleep(REQUEST_INTERVAL_MS * 1000);
    }

    return NULL;
}

static void *rx_thread(void *arg)
{
    obd2_drv_t *drv = (obd2_drv_t *)arg;
    struct can_frame frame;
    int timeout_count = 0;

    while (drv->running) {
        int nbytes = read(drv->sockfd, &frame, sizeof(frame));
        if (nbytes < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                timeout_count++;
                if (timeout_count > 3) {
                    /* 连续超时标记断连 */
                    pthread_mutex_lock(&drv->data_mutex);
                    drv->obd2.connected = 0;
                    pthread_mutex_unlock(&drv->data_mutex);
                    timeout_count = 0;
                }
                continue;
            }
            break;
        }

        if (nbytes != sizeof(frame)) continue;
        timeout_count = 0;

        if (frame.can_id >= OBD2_RESPONSE_ID_MIN &&
            frame.can_id <= OBD2_RESPONSE_ID_MAX) {
            drv->rx_count++;

            pthread_mutex_lock(&drv->data_mutex);
            drv->obd2.connected = 1;
            if (parse_obd2_response(&frame, &drv->obd2) == 0) {
                /* 映射到 Encoder_Data_t 兼容原有接口 */
                drv->encoder.count     = (int16_t)drv->obd2.rpm;
                drv->encoder.speed_rpm = drv->obd2.speed_kmh;
                drv->encoder.angle     = drv->obd2.throttle_pct;
                drv->encoder.dir       = 0;
            }
            pthread_mutex_unlock(&drv->data_mutex);
        }
    }

    return NULL;
}

// ========== 公共 API ==========

int obd2_drv_init(obd2_drv_t *drv)
{
    if (!drv) return -1;

    memset(drv, 0, sizeof(*drv));
    pthread_mutex_init(&drv->data_mutex, NULL);

    if (socketcan_init(drv) != 0) {
        drv->state = OBD2_STATE_ERROR;
        return -1;
    }

    drv->running = true;
    drv->state   = OBD2_STATE_RUNNING;

    if (pthread_create(&drv->tx_tid, NULL, tx_thread, drv) != 0) {
        perror("[obd2] tx_thread");
        close(drv->sockfd);
        drv->state = OBD2_STATE_ERROR;
        return -1;
    }

    if (pthread_create(&drv->rx_tid, NULL, rx_thread, drv) != 0) {
        perror("[obd2] rx_thread");
        drv->running = false;
        pthread_join(drv->tx_tid, NULL);
        close(drv->sockfd);
        drv->state = OBD2_STATE_ERROR;
        return -1;
    }

    printf("[obd2] driver initialized on %s\n", CAN_IFNAME);
    return 0;
}

int obd2_drv_get_encoder(obd2_drv_t *drv, Encoder_Data_t *enc)
{
    if (!drv || !enc) return -1;

    pthread_mutex_lock(&drv->data_mutex);
    *enc = drv->encoder;
    pthread_mutex_unlock(&drv->data_mutex);

    return 0;
}

int obd2_drv_get_obd2(obd2_drv_t *drv, Obd2_Data_t *data)
{
    if (!drv || !data) return -1;

    pthread_mutex_lock(&drv->data_mutex);
    *data = drv->obd2;
    pthread_mutex_unlock(&drv->data_mutex);

    return 0;
}

void obd2_drv_deinit(obd2_drv_t *drv)
{
    if (!drv) return;

    drv->running = false;
    pthread_join(drv->tx_tid, NULL);
    pthread_join(drv->rx_tid, NULL);

    if (drv->sockfd >= 0) {
        close(drv->sockfd);
        drv->sockfd = -1;
    }

    pthread_mutex_destroy(&drv->data_mutex);
    drv->state = OBD2_STATE_IDLE;

    printf("[obd2] driver deinitialized\n");
}