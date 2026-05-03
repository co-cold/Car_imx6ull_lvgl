#ifndef __CAMERA_HARDWARE_H__
#define __CAMERA_HARDWARE_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

// 摄像头硬件配置
typedef struct {
    const char *device;      // 设备路径
    int width;               // 宽度
    int height;              // 高度
    int fps;                 // 帧率
} camera_hw_config_t;

// 摄像头硬件状态
typedef enum {
    CAMERA_HW_IDLE = 0,
    CAMERA_HW_INITIALIZED,
    CAMERA_HW_STREAMING,
    CAMERA_HW_ERROR
} camera_hw_state_t;

// 摄像头硬件管理器
typedef struct {
    // 配置
    camera_hw_config_t config;
    camera_hw_state_t state;
    
    // 硬件句柄
    int fd;                    // 文件描述符
    pthread_t capture_thread;  // 采集线程
    bool thread_running;       // 线程运行标志
    
    // 缓冲区
    uint8_t *frame_buffers[2];
    bool frame_ready[2]; // 每个缓冲区各自的就绪标志
    pthread_mutex_t buffer_mutex;  // 保护缓冲区交换
    
    int frame_count;
    float actual_fps;

    // ============ 添加I422转换缓冲区 ============
    uint8_t *y_plane;     // Y平面缓冲区
    uint8_t *u_plane;     // U平面缓冲区
    uint8_t *v_plane;     // V平面缓冲区
    int y_plane_size;     // Y平面大小
    int uv_plane_size;    // U/V平面大小

} camera_hardware_t;

// 硬件操作接口
camera_hardware_t* camera_hw_create(const char *device, int width, int height, int fps);
int camera_hw_init(camera_hardware_t *camera);
int camera_hw_start(camera_hardware_t *camera);
int camera_hw_stop(camera_hardware_t *camera);
void camera_hw_destroy(camera_hardware_t *camera);

// 帧获取接口
int camera_hw_get_latest_frame(camera_hardware_t *camera, uint8_t *output_buffer);

// 控制接口（需要V4L2扩展支持）
int camera_hw_set_brightness(camera_hardware_t *camera, int value);
int camera_hw_set_contrast(camera_hardware_t *camera, int value);
int camera_hw_set_exposure(camera_hardware_t *camera, int value);

// 工具函数
const char* camera_hw_state_to_string(camera_hw_state_t state);
float camera_hw_get_fps(camera_hardware_t *camera);
int camera_hw_get_frame_count(camera_hardware_t *camera);

#endif