#include "camera_hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/videodev2.h>

#include "libyuv.h" 

// 内部函数声明
static void* camera_capture_thread_func(void *arg);
static int v4l2_init_camera(camera_hardware_t *camera);
static int v4l2_start_capture(camera_hardware_t *camera);
static int v4l2_stop_capture(camera_hardware_t *camera);
static void v4l2_cleanup(camera_hardware_t *camera);
static void yuyv_to_rgb565(camera_hardware_t *camera, uint8_t *yuyv, uint8_t *rgb);

// ============ 添加缺少的V4L2缓冲区管理 ============
typedef struct {
    void *start;
    size_t length;
} v4l2_mapped_buffer_t;

static v4l2_mapped_buffer_t v4l2_buffers[4];
static int v4l2_buffer_count = 0;

// ============ 添加YUYV到RGB565的转换函数 ============
static void yuyv_to_rgb565(camera_hardware_t *camera, uint8_t *yuyv, uint8_t *rgb)
{
    int width = camera->config.width;
    int height = camera->config.height;

    // // 检查输入YUYV数据
    // static int check_count = 0;
    // check_count++;

    // if (check_count % 20 == 0) {
    //     printf("[HW-CONV] 输入YUYV前20字节: ");
    //     for (int i = 0; i < 20; i++) {
    //         printf("%02X ", yuyv[i]);
    //     }
    //     printf(" (帧#%d)\n", check_count);
    // }
    
    // 使用YUY2ToI420将YUYV转换为I420
    YUY2ToI420(yuyv, width * 2,              // 输入YUYV数据和stride
               camera->y_plane, width,       // Y平面输出
               camera->u_plane, width / 2,   // U平面输出
               camera->v_plane, width / 2,   // V平面输出
               width, height);
    
    // 使用I420ToRGB565转换为RGB565
    I420ToRGB565(camera->y_plane, width,     // Y平面
                 camera->u_plane, width / 2, // U平面
                 camera->v_plane, width / 2, // V平面
                 rgb, width * 2,             // 输出RGB565
                 width, height);

    // if (check_count % 20 == 0) {
    //     uint16_t *rgb16 = (uint16_t*)rgb;
    //     printf("[HW-CONV] 输出RGB565前20像素: ");
    //     for (int i = 0; i < 20; i++) {
    //         printf("0x%04X ", rgb16[i]);
    //     }
    //     printf("\n");
    // }
}

// ============ 完整的V4L2初始化 ============
static int v4l2_init_camera(camera_hardware_t *camera) 
{
    if (!camera) return -1;
    
    printf("[HW] V4L2初始化摄像头: %s\n", camera->config.device);
    
    // 1. 打开设备
    camera->fd = open(camera->config.device, O_RDWR);
    if (camera->fd < 0) {
        perror("[HW] 打开摄像头失败");
        return -1;
    }
    
    // 2. 设置格式
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = camera->config.width;
    fmt.fmt.pix.height = camera->config.height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    
    if (ioctl(camera->fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("[HW] 设置格式失败");
        close(camera->fd);
        return -1;
    }

    // 设置格式后，添加帧率设置
    struct v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    if (ioctl(camera->fd, VIDIOC_G_PARM, &streamparm) == 0) {
        printf("[HW] 摄像头支持能力: 0x%08x\n", streamparm.parm.capture.capability);
        
        // 设置帧率
        streamparm.parm.capture.timeperframe.numerator = 1;
        streamparm.parm.capture.timeperframe.denominator = camera->config.fps;
        
        if (ioctl(camera->fd, VIDIOC_S_PARM, &streamparm) < 0) {
            perror("[HW] 设置帧率失败，使用默认帧率");
        } else {
            printf("[HW] 设置帧率: %d FPS (实际: %d/%d)\n", 
                   camera->config.fps,
                   streamparm.parm.capture.timeperframe.denominator,
                   streamparm.parm.capture.timeperframe.numerator);
        }
    }
    
    // 3. 申请缓冲区
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.count = 4;
    req.memory = V4L2_MEMORY_MMAP;
    
    if (ioctl(camera->fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("[HW] 申请缓冲区失败");
        close(camera->fd);
        return -1;
    }
    
    v4l2_buffer_count = req.count;
    
    // 4. 映射缓冲区
    for (int i = 0; i < v4l2_buffer_count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        
        if (ioctl(camera->fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("[HW] 查询缓冲区失败");
            close(camera->fd);
            return -1;
        }
        
        v4l2_buffers[i].length = buf.length;
        v4l2_buffers[i].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      camera->fd,
                                      buf.m.offset);
        
        if (v4l2_buffers[i].start == MAP_FAILED) {
            perror("[HW] 映射缓冲区失败");
            close(camera->fd);
            return -1;
        }
        
        // 将缓冲区放入队列
        if (ioctl(camera->fd, VIDIOC_QBUF, &buf) < 0) {
            perror("[HW] 缓冲区入队失败");
            close(camera->fd);
            return -1;
        }
    }
    
    printf("[HW] V4L2初始化完成，缓冲区数量: %d\n", v4l2_buffer_count);
    return 0;
}

// ============ 启动V4L2采集 ============
static int v4l2_start_capture(camera_hardware_t *camera) 
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    if (ioctl(camera->fd, VIDIOC_STREAMON, &type) < 0) {
        perror("[HW] 启动视频流失败");
        return -1;
    }
    
    return 0;
}

// ============ 停止V4L2采集 ============
static int v4l2_stop_capture(camera_hardware_t *camera) 
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    if (ioctl(camera->fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("[HW] 停止视频流失败");
        return -1;
    }
    
    return 0;
}

// ============ 清理V4L2资源 ============
static void v4l2_cleanup(camera_hardware_t *camera) 
{
    // 取消映射
    for (int i = 0; i < v4l2_buffer_count; i++) {
        if (v4l2_buffers[i].start) {
            munmap(v4l2_buffers[i].start, v4l2_buffers[i].length);
        }
    }
    
    // 关闭文件描述符
    if (camera->fd >= 0) {
        close(camera->fd);
        camera->fd = -1;
    }
}

// ============ 增强的摄像头采集线程 ============
static void* camera_capture_thread_func(void *arg) 
{
    camera_hardware_t *camera = (camera_hardware_t *)arg;
    
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    int local_frame_count = 0;
    
    while (camera->thread_running) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        
        if (ioctl(camera->fd, VIDIOC_DQBUF, &buf) < 0) {
            continue;
        }
        
        uint8_t *yuyv_data = (uint8_t*)v4l2_buffers[buf.index].start;
        
        pthread_mutex_lock(&camera->buffer_mutex);

        int buf_to_write = buf.index % 2;
        
        yuyv_to_rgb565(camera, yuyv_data, camera->frame_buffers[buf_to_write]);
        
        camera->frame_ready[buf_to_write] = true;
        
        camera->frame_count++;
        local_frame_count++;
        
        pthread_mutex_unlock(&camera->buffer_mutex);
        
        if (ioctl(camera->fd, VIDIOC_QBUF, &buf) < 0) {
            continue;
        }

        // // 检测是否为全黑帧
        // uint16_t *pixels = (uint16_t*)camera->frame_buffers[buf_to_write];
        // int black_pixel_count = 0;
        // for (int i = 0; i < 100; i++) { // 检查前100个像素
        //     if (pixels[i] == 0x0000) black_pixel_count++;
        // }
        
        // if (black_pixel_count > 90) { // 如果90%以上是黑色
        //     printf("[HW-WARN] 检测到可能全黑帧 (黑像素: %d/100)\n", black_pixel_count);
        // }
        
        // // 只保留帧率统计
        // if (local_frame_count % 30 == 0) {
        //     gettimeofday(&end_time, NULL);
        //     double elapsed = (end_time.tv_sec - start_time.tv_sec) + 
        //                     (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        //     camera->actual_fps = local_frame_count / elapsed;
            
        //     printf("[HW] 帧率: %.2f FPS\n", camera->actual_fps);
            
        //     start_time = end_time;
        //     local_frame_count = 0;
        // }
    }
    return NULL;
}

// ============ 接口函数 V4L2代码 ============
camera_hardware_t* camera_hw_create(const char *device, int width, int height, int fps) 
{
    camera_hardware_t *camera = malloc(sizeof(camera_hardware_t));
    if (!camera) return NULL;
    
    memset(camera, 0, sizeof(camera_hardware_t));
    
    camera->config.device = strdup(device);
    camera->config.width = width;
    camera->config.height = height;
    camera->config.fps = fps;
    camera->state = CAMERA_HW_IDLE;
    camera->fd = -1;
    
    // 初始化互斥锁
    pthread_mutex_init(&camera->buffer_mutex, NULL);
    
    // ============ 分配双缓冲区内存 ============
    int buffer_size = width * height * 2;  // RGB565
    camera->frame_buffers[0] = malloc(buffer_size);
    camera->frame_buffers[1] = malloc(buffer_size);
    
    if (!camera->frame_buffers[0] || !camera->frame_buffers[1]) {
        printf("[HW] 分配双缓冲区失败\n");
        if (camera->frame_buffers[0]) free(camera->frame_buffers[0]);
        if (camera->frame_buffers[1]) free(camera->frame_buffers[1]);
        free(camera);
        return NULL;
    }
    
    // 初始化为黑色
    memset(camera->frame_buffers[0], 0, buffer_size);
    memset(camera->frame_buffers[1], 0, buffer_size);
    
    // 初始化双缓冲区状态
    camera->frame_ready[0] = false;
    camera->frame_ready[1] = false;

    // ============ 预分配I422转换缓冲区 ============
    // I422格式
    camera->y_plane_size = width * height;
    camera->uv_plane_size = (width / 2) * (height / 2); 
    
    camera->y_plane = malloc(camera->y_plane_size);
    camera->u_plane = malloc(camera->uv_plane_size);
    camera->v_plane = malloc(camera->uv_plane_size);
    
    if (!camera->y_plane || !camera->u_plane || !camera->v_plane) {
        printf("[HW] 分配I422转换缓冲区失败\n");
        // 清理已分配的资源
        if (camera->y_plane) free(camera->y_plane);
        if (camera->u_plane) free(camera->u_plane);
        if (camera->v_plane) free(camera->v_plane);
        if (camera->frame_buffers[0]) free(camera->frame_buffers[0]);
        if (camera->frame_buffers[1]) free(camera->frame_buffers[1]);
        free(camera);
        return NULL;
    }
    
    printf("[HW] 摄像头硬件实例创建: %s, %dx%d, %dFPS\n", device, width, height, fps);
    
    return camera;
}

// 初始化摄像头硬件
int camera_hw_init(camera_hardware_t *camera) 
{
    if (!camera) return -1;
    
    printf("[HW] 初始化摄像头硬件...\n");
    
    if (v4l2_init_camera(camera) < 0) {
        camera->state = CAMERA_HW_ERROR;
        return -1;
    }
    
    camera->state = CAMERA_HW_INITIALIZED;
    printf("[HW] 摄像头硬件初始化完成\n");
    
    return 0;
}

// 启动摄像头采集
int camera_hw_start(camera_hardware_t *camera) 
{
    if (!camera || camera->state != CAMERA_HW_INITIALIZED) {
        return -1;
    }
    
    if (v4l2_start_capture(camera) < 0) {
        return -1;
    }
    
    camera->thread_running = true;
    
    // 创建采集线程
    int ret = pthread_create(&camera->capture_thread, NULL, 
                            camera_capture_thread_func, camera);
    if (ret != 0) {
        printf("[HW] 创建采集线程失败: %d\n", ret);
        camera->thread_running = false;
        v4l2_stop_capture(camera);
        return -1;
    }
    
    #ifdef __linux__
    pthread_setname_np(camera->capture_thread, "camera-capture");
    #endif
    
    camera->state = CAMERA_HW_STREAMING;
    printf("[HW] 摄像头采集已启动\n");
    
    return 0;
}

// 停止摄像头
int camera_hw_stop(camera_hardware_t *camera) 
{
    if (!camera || camera->state != CAMERA_HW_STREAMING) {
        return -1;
    }
    
    camera->thread_running = false;
    
    // 等待线程结束
    if (camera->capture_thread) {
        pthread_join(camera->capture_thread, NULL);
    }
    
    v4l2_stop_capture(camera);
    camera->state = CAMERA_HW_INITIALIZED;
    printf("[HW] 摄像头采集已停止\n");
    
    return 0;
}

// 销毁摄像头
void camera_hw_destroy(camera_hardware_t *camera) 
{
    if (!camera) return;
    
    camera_hw_stop(camera);
    
    // 清理V4L2资源
    v4l2_cleanup(camera);

    // 释放I422转换缓冲区
    if (camera->y_plane) {
        free(camera->y_plane);
        camera->y_plane = NULL;
    }
    if (camera->u_plane) {
        free(camera->u_plane);
        camera->u_plane = NULL;
    }
    if (camera->v_plane) {
        free(camera->v_plane);
        camera->v_plane = NULL;
    }
    
    // 释放双缓冲区
    if (camera->frame_buffers[0]) {
        free(camera->frame_buffers[0]);
        camera->frame_buffers[0] = NULL;
    }
    if (camera->frame_buffers[1]) {
        free(camera->frame_buffers[1]);
        camera->frame_buffers[1] = NULL;
    }
    
    // 销毁互斥锁
    pthread_mutex_destroy(&camera->buffer_mutex);
    
    // 释放设备字符串
    if (camera->config.device) {
        free((void*)camera->config.device);
    }
    
    free(camera);
    printf("[HW] 摄像头硬件已销毁\n");
}

int camera_hw_get_latest_frame(camera_hardware_t *camera, uint8_t *output_buffer) 
{
    if (!camera || !output_buffer) return -1;
    
    int ret = -1;
    int buffer_size = camera->config.width * camera->config.height * 2;
    
    pthread_mutex_lock(&camera->buffer_mutex);
    
    int found_buffer = -1;
    for (int i = 0; i < 2; i++) {
        if (camera->frame_ready[i]) {
            found_buffer = i;
            break;
        }
    }
    
    if (found_buffer != -1) {
        memcpy(output_buffer, camera->frame_buffers[found_buffer], buffer_size);
        
        // // 调试：检查前几个像素
        // static int frame_count = 0;
        // frame_count++;
        // if (frame_count % 10 == 0) {
        //     uint16_t *pixels = (uint16_t*)output_buffer;
        //     printf("帧#%d 前20像素:", frame_count);
        //     for(int i = 0; i < 20; i++)
        //     {
        //         printf("0x%04X ", pixels[i]);
        //     }
        //     printf("\n");
        // }
        
        camera->frame_ready[found_buffer] = false;
        ret = 0;
    }
    
    pthread_mutex_unlock(&camera->buffer_mutex);
    
    return ret;
}

// 工具函数实现
const char* camera_hw_state_to_string(camera_hw_state_t state) 
{
    switch (state) {
        case CAMERA_HW_IDLE: return "IDLE";
        case CAMERA_HW_INITIALIZED: return "INITIALIZED";
        case CAMERA_HW_STREAMING: return "STREAMING";
        case CAMERA_HW_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

float camera_hw_get_fps(camera_hardware_t *camera) 
{
    return camera ? camera->actual_fps : 0.0f;
}

int camera_hw_get_frame_count(camera_hardware_t *camera) 
{
    return camera ? camera->frame_count : 0;
}