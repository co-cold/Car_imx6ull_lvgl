#include "uwb_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>

static int compare_u32(const void *a, const void *b)
{
    uint32_t va = *(const uint32_t *)a;
    uint32_t vb = *(const uint32_t *)b;
    if (va < vb) return -1;
    if (va > vb) return  1;
    return 0;
}

static uint32_t uwb_median_filter(uwb_drv_t *drv, uint32_t raw)
{
    drv->median_buf[drv->median_idx] = raw;
    drv->median_idx = (drv->median_idx + 1) % UWB_MEDIAN_WINDOW;
    if (drv->median_count < UWB_MEDIAN_WINDOW)
        drv->median_count++;

    if (drv->median_count < UWB_MEDIAN_WINDOW)
        return raw;

    uint32_t sorted[UWB_MEDIAN_WINDOW];
    memcpy(sorted, drv->median_buf, sizeof(sorted));
    qsort(sorted, UWB_MEDIAN_WINDOW, sizeof(uint32_t), compare_u32);
    return sorted[UWB_MEDIAN_WINDOW / 2];
}

static uint32_t uwb_moving_average(uwb_drv_t *drv, uint32_t filtered)
{
    uint32_t oldest = drv->avg_buf[drv->avg_idx];

    drv->avg_buf[drv->avg_idx] = filtered;
    drv->avg_idx = (drv->avg_idx + 1) % UWB_AVERAGE_WINDOW;

    if (drv->avg_count < UWB_AVERAGE_WINDOW) {
        drv->avg_count++;
        drv->avg_sum += filtered;
    } else {
        drv->avg_sum = drv->avg_sum - oldest + filtered;
    }

    return drv->avg_sum / drv->avg_count;
}

static int uwb_parse_line(const char *line, uint32_t *dist_mm, uint16_t *seq, int *rssi, uint32_t *raw_ts)
{
    char     type[4] = {0};
    int      tag_id  = 0;
    unsigned dist_hex = 0;
    unsigned res1 = 0, res2 = 0, res3 = 0;
    unsigned seq_hex  = 0;
    int      rssi_val = 0;
    unsigned ts_hex   = 0;

    int n = sscanf(line, "%3s %d %x %x %x %x %x %d %x",
                   type, &tag_id, &dist_hex, &res1, &res2, &res3,
                   &seq_hex, &rssi_val, &ts_hex);

    if (n < 9) return -1;

    if (strcmp(type, "mc") != 0) return -1;

    *dist_mm = (uint32_t)dist_hex;
    *seq     = (uint16_t)seq_hex;
    *rssi    = rssi_val;
    *raw_ts  = ts_hex;
    return 0;
}

static uint64_t uwb_get_time_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

static void *uwb_rx_thread(void *arg)
{
    uwb_drv_t *drv = (uwb_drv_t *)arg;
    char line[256];
    int  line_pos = 0;

    while (drv->running) {
        char ch;
        int ret = read(drv->fd, &ch, 1);
        if (ret <= 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            fprintf(stderr, "[uwb_drv] serial read error: %s\n", strerror(errno));
            drv->state = UWB_STATE_ERROR;
            break;
        }

        if (ch == '\n' || ch == '\r') {
            if (line_pos > 0) {
                line[line_pos] = '\0';
                line_pos = 0;

                uint32_t dist_mm = 0;
                uint16_t seq     = 0;
                int      rssi    = 0;
                uint32_t raw_ts  = 0;

                if (uwb_parse_line(line, &dist_mm, &seq, &rssi, &raw_ts) == 0) {
                    uint64_t now_us = uwb_get_time_us();

                    uint32_t filtered = uwb_median_filter(drv, dist_mm);
                    uint32_t averaged = uwb_moving_average(drv, filtered);

                    pthread_mutex_lock(&drv->data_mutex);

                    drv->data.distance_mm          = dist_mm;
                    drv->data.distance_filtered_mm = averaged;
                    drv->data.seq                  = seq;
                    drv->data.rssi                 = rssi;
                    drv->data.raw_timestamp        = raw_ts;
                    drv->data.timestamp_ms         = (uint32_t)(now_us / 1000);

                    if (drv->last_timestamp_us > 0 && drv->last_distance_mm > 0) {
                        double dt_s = (double)(now_us - drv->last_timestamp_us) / 1000000.0;
                        if (dt_s > 0.001) {
                            double delta_mm = (double)averaged - (double)drv->last_distance_mm;
                            drv->data.distance_change_rate = (float)(delta_mm / dt_s);
                        }
                    }

                    drv->last_distance_mm = averaged;
                    drv->last_timestamp_us = now_us;

                    pthread_mutex_unlock(&drv->data_mutex);
                    drv->rx_count++;
                }
            }
        } else {
            if (line_pos < (int)sizeof(line) - 1)
                line[line_pos++] = ch;
        }
    }
    return NULL;
}

int uwb_drv_init(uwb_drv_t *drv, const char *device, int baudrate)
{
    if (!drv || !device) return -1;

    memset(drv, 0, sizeof(*drv));
    strncpy(drv->device, device, sizeof(drv->device) - 1);
    drv->baudrate = baudrate;

    drv->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (drv->fd < 0) {
        fprintf(stderr, "[uwb_drv] open %s failed: %s\n", device, strerror(errno));
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(drv->fd, &tty) != 0) {
        fprintf(stderr, "[uwb_drv] tcgetattr failed: %s\n", strerror(errno));
        close(drv->fd);
        return -1;
    }

    speed_t speed;
    switch (baudrate) {
        case 115200: speed = B115200; break;
        case 921600: speed = B921600; break;
        case 460800: speed = B460800; break;
        case 230400: speed = B230400; break;
        default:
            fprintf(stderr, "[uwb_drv] unsupported baudrate: %d\n", baudrate);
            close(drv->fd);
            return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(drv->fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "[uwb_drv] tcsetattr failed: %s\n", strerror(errno));
        close(drv->fd);
        return -1;
    }

    tcflush(drv->fd, TCIOFLUSH);

    pthread_mutex_init(&drv->data_mutex, NULL);
    drv->running = true;
    drv->state   = UWB_STATE_RUNNING;

    if (pthread_create(&drv->rx_tid, NULL, uwb_rx_thread, drv) != 0) {
        fprintf(stderr, "[uwb_drv] pthread_create rx failed\n");
        drv->running = false;
        close(drv->fd);
        return -1;
    }

    printf("[uwb_drv] initialized, device=%s, baudrate=%d\n", device, baudrate);
    return 0;
}

int uwb_drv_get_data(uwb_drv_t *drv, Uwb_Data_t *data)
{
    if (!drv || !data) return -1;
    pthread_mutex_lock(&drv->data_mutex);
    *data = drv->data;
    pthread_mutex_unlock(&drv->data_mutex);
    return 0;
}

void uwb_drv_deinit(uwb_drv_t *drv)
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
    printf("[uwb_drv] deinitialized, rx=%u err=%u\n", drv->rx_count, drv->err_count);
}