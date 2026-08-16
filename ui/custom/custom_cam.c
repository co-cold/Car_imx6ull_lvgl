#include "custom_cam.h"
#include "ipc/ipc_camera.h"
#include "gui_guider.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUS_ADDRESS "unix:path=/tmp/lvgl-dbus-session"

// 全局摄像头UI管理器
static camera_ui_t *cam_ui = NULL;
static pid_t g_camera_svc_pid = 0;

// 状态转字符串函数
const char* camera_ui_state_to_string(camera_ui_state_t state) {
    switch (state) {
        case CAMERA_UI_IDLE: return "IDLE";
        case CAMERA_UI_INITIALIZED: return "INITIALIZED";
        case CAMERA_UI_RUNNING: return "RUNNING";
        case CAMERA_UI_STOPPING: return "STOPPING";
        case CAMERA_UI_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// LVGL显示更新定时器回调
static void camera_display_timer_cb(lv_timer_t *timer) {
    camera_ui_t *camera_ui = timer->user_data;
    
    if (!camera_ui) {
        return;
    }

    if (ipc_camera_get_frame(camera_ui->display_buffer, camera_ui->buffer_size) == 0) {
        lv_obj_invalidate(camera_ui->display_img);
    }
}

// 初始化摄像头UI
camera_ui_t* camera_ui_init(lv_obj_t *display_img, const char *device, int width, int height, int fps) {
    printf("=== 初始化摄像头UI ===\n");
    
    if (!display_img) {
        printf("错误: 显示图片组件为空\n");
        return NULL;
    }
    
    // 如果已经有实例，先销毁
    if (cam_ui) {
        printf("警告: 摄像头UI已存在，先销毁旧的\n");
        camera_ui_deinit(cam_ui);
    }

    // 先杀掉可能残留的旧 camera_service
    if (g_camera_svc_pid > 0) {
        kill(g_camera_svc_pid, SIGTERM);
        waitpid(g_camera_svc_pid, NULL, 0);
        g_camera_svc_pid = 0;
    }

    // 拉起 camera_service 进程
    pid_t pid = fork();
    if (pid == 0) {
        execl("./camera_service", "camera_service", NULL);
        perror("execl camera_service");
        _exit(1);
    } else if (pid > 0) {
        g_camera_svc_pid = pid;
        printf("camera_service started, pid=%d\n", pid);
    }

    // 等待 camera_service 初始化完成（硬件 + D-Bus 注册）
    usleep(1000000);

    // 初始化 IPC 连接
    if (ipc_camera_init(BUS_ADDRESS, width, height, NULL, NULL) != 0) {
        printf("警告: IPC Camera 初始化失败，摄像头不可用\n");
    }
    
    // 创建摄像头UI结构
    camera_ui_t *camera_ui = malloc(sizeof(camera_ui_t));
    if (!camera_ui) {
        printf("错误: 分配摄像头UI结构失败\n");
        return NULL;
    }
    
    memset(camera_ui, 0, sizeof(camera_ui_t));
    camera_ui->state = CAMERA_UI_IDLE;
    
    // 计算缓冲区大小
    camera_ui->buffer_size = width * height * 2;  // RGB565
    
    // 分配显示缓冲区
    camera_ui->display_buffer = malloc(camera_ui->buffer_size);
    if (!camera_ui->display_buffer) {
        printf("错误: 分配显示缓冲区失败\n");
        free(camera_ui);
        return NULL;
    }
    
    // 初始化为黑色
    memset(camera_ui->display_buffer, 0, camera_ui->buffer_size);
    
    // 设置图片源
    camera_ui->display_img = display_img;
    
    // 初始化图片描述符
    camera_ui->img_desc.header.cf = LV_IMG_CF_TRUE_COLOR;  // RGB565
    camera_ui->img_desc.header.w = width;
    camera_ui->img_desc.header.h = height;
    camera_ui->img_desc.data_size = camera_ui->buffer_size;
    camera_ui->img_desc.data = camera_ui->display_buffer;
    
    // 设置图片源
    lv_img_set_src(display_img, &camera_ui->img_desc);
    
    // 创建显示更新定时器
    camera_ui->update_timer = lv_timer_create(camera_display_timer_cb, 30, camera_ui); 
    
    camera_ui->state = CAMERA_UI_INITIALIZED;
    cam_ui = camera_ui;  // 保存全局实例
    
    printf("摄像头UI初始化完成，状态=%s\n", camera_ui_state_to_string(camera_ui->state));
    printf("图片组件地址: %p, 缓冲区地址: %p\n", 
           (void*)display_img, (void*)camera_ui->display_buffer);
    
    return camera_ui;
}

// 反初始化摄像头UI
void camera_ui_deinit(camera_ui_t *camera_ui) {
    printf("=== 反初始化摄像头UI ===\n");
    
    // 如果传入NULL，使用全局实例
    if (camera_ui == NULL) {
        camera_ui = cam_ui;
        printf("使用全局实例: cam_ui=%p\n", (void*)camera_ui);
    }
    
    if (!camera_ui) {
        printf("警告: 摄像头UI为空，无需反初始化\n");
        return;
    }
    
    printf("当前状态: %s\n", camera_ui_state_to_string(camera_ui->state));
    
    // 1. 先删除定时器，避免回调使用已释放的内存
    if (camera_ui->update_timer) {
        lv_timer_del(camera_ui->update_timer);
        camera_ui->update_timer = NULL;
        printf("已删除LVGL定时器\n");
    }
    
    // 2. 停止摄像头（通过 IPC）
    if (camera_ui->state == CAMERA_UI_RUNNING) {
        printf("摄像头正在运行，先停止\n");
        camera_ui_stop(camera_ui);
    }
    
    // 3. 清理LVGL图片源
    if (camera_ui->display_img) {
        lv_img_set_src(camera_ui->display_img, NULL);
        camera_ui->display_img = NULL;
        printf("已清除LVGL图片源\n");
    }
    
    // 4. 释放缓冲区
    if (camera_ui->display_buffer) {
        free(camera_ui->display_buffer);
        camera_ui->display_buffer = NULL;
    }
    
    // 5. 记录并清除全局变量
    camera_ui_state_t old_state = camera_ui->state;
    
    if (cam_ui == camera_ui) {
        cam_ui = NULL;
        printf("已清除全局变量cam_ui\n");
    }
    
    // 6. IPC 反初始化
    ipc_camera_deinit();

    // 7. 杀掉 camera_service 进程
    if (g_camera_svc_pid > 0) {
        kill(g_camera_svc_pid, SIGKILL);
        int retries = 20;
        while (retries-- > 0) {
            if (waitpid(g_camera_svc_pid, NULL, WNOHANG) != 0)
                break;
            usleep(50000);
        }
        if (retries <= 0) {
            printf("警告: camera_service 未响应，强制放弃\n");
        }
        printf("camera_service stopped, pid=%d\n", g_camera_svc_pid);
        g_camera_svc_pid = 0;
    }

    // 8. 最后释放结构体
    free(camera_ui);
    
    printf("摄像头UI已反初始化，之前状态=%s\n", camera_ui_state_to_string(old_state));
}

// 启动摄像头UI
int camera_ui_start(camera_ui_t *camera_ui) {
    printf("=== 启动摄像头UI ===\n");
    
    // 如果传入NULL，使用全局实例
    if (camera_ui == NULL) {
        camera_ui = cam_ui;
        printf("使用全局实例: cam_ui=%p\n", (void*)camera_ui);
    }
    
    if (!camera_ui) {
        printf("错误: 摄像头UI为空\n");
        return -1;
    }
    
    if (camera_ui->state != CAMERA_UI_INITIALIZED && camera_ui->state != CAMERA_UI_STOPPING) {
        printf("错误: 状态不正确，当前状态=%s，期望状态=INITIALIZED或STOPPING\n", 
               camera_ui_state_to_string(camera_ui->state));
        return -1;
    }
    
    // 启动摄像头（通过 IPC）
    if (ipc_camera_start() < 0) {
        printf("错误: 启动摄像头失败\n");
        camera_ui->state = CAMERA_UI_ERROR;
        return -1;
    }
    
    camera_ui->state = CAMERA_UI_RUNNING;
    printf("摄像头UI已启动，状态=%s\n", camera_ui_state_to_string(camera_ui->state));
    
    return 0;
}

// 停止摄像头UI
int camera_ui_stop(camera_ui_t *camera_ui) {
    printf("=== 停止摄像头UI ===\n");
    
    // 如果传入NULL，使用全局实例
    if (camera_ui == NULL) {
        camera_ui = cam_ui;
        printf("使用全局实例: cam_ui=%p\n", (void*)camera_ui);
    }
    
    if (!camera_ui) {
        printf("错误: 摄像头UI为空\n");
        return -1;
    }
    
    if (camera_ui->state != CAMERA_UI_RUNNING) {
        printf("警告: 摄像头未在运行，状态=%s\n", camera_ui_state_to_string(camera_ui->state));
        return 0;
    }
    
    // 停止摄像头（通过 IPC）
    if (ipc_camera_stop() < 0) {
        printf("错误: 停止摄像头失败\n");
        camera_ui->state = CAMERA_UI_ERROR;
        return -1;
    }
    
    camera_ui->state = CAMERA_UI_STOPPING;
    printf("摄像头UI已停止，状态=%s\n", camera_ui_state_to_string(camera_ui->state));
    
    return 0;
}

// 获取摄像头状态
camera_ui_state_t camera_ui_get_state(camera_ui_t *camera_ui) {
    return camera_ui ? camera_ui->state : CAMERA_UI_IDLE;
}