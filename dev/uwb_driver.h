#ifndef _UWB_DRIVER_H_
#define _UWB_DRIVER_H_

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#define UWB_MEDIAN_WINDOW   5
#define UWB_AVERAGE_WINDOW  10

typedef struct {
    uint32_t distance_mm;
    uint32_t distance_filtered_mm;
    float    distance_change_rate;
    int      rssi;
    uint32_t raw_timestamp;
    uint16_t seq;
    uint32_t timestamp_ms;
} Uwb_Data_t;

typedef enum {
    UWB_STATE_IDLE,
    UWB_STATE_RUNNING,
    UWB_STATE_ERROR
} uwb_state_t;

typedef struct {
    char     device[64];
    int      fd;
    int      baudrate;

    pthread_t        rx_tid;
    volatile bool    running;

    Uwb_Data_t       data;
    pthread_mutex_t  data_mutex;

    uint32_t         rx_count;
    uint32_t         err_count;
    uwb_state_t      state;

    uint32_t         median_buf[UWB_MEDIAN_WINDOW];
    int              median_idx;
    int              median_count;

    uint32_t         avg_buf[UWB_AVERAGE_WINDOW];
    int              avg_idx;
    int              avg_count;
    uint32_t         avg_sum;

    uint32_t         last_distance_mm;
    uint64_t         last_timestamp_us;
} uwb_drv_t;

int  uwb_drv_init(uwb_drv_t *drv, const char *device, int baudrate);
int  uwb_drv_get_data(uwb_drv_t *drv, Uwb_Data_t *data);
void uwb_drv_deinit(uwb_drv_t *drv);

#endif