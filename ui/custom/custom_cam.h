#ifndef __CUSTOM_CAM_H__
#define __CUSTOM_CAM_H__

#include "lvgl.h"
#include "gui_guider.h"
#include "camera_hardware.h"

// 摄像头UI状态
typedef enum {
    CAMERA_UI_IDLE = 0,        // 空闲状态
    CAMERA_UI_INITIALIZED,     // 已初始化但未启动
    CAMERA_UI_RUNNING,         // 正在运行
    CAMERA_UI_STOPPING,        // 停止中
    CAMERA_UI_ERROR           // 错误状态
} camera_ui_state_t;

// 摄像头UI管理器
typedef struct {
    camera_hardware_t *hw_camera;
    lv_obj_t *display_img;
    lv_img_dsc_t img_desc;
    lv_timer_t *update_timer;
    
    uint8_t *display_buffer;
    int buffer_size;
    
    camera_ui_state_t state;
    
} camera_ui_t;

// 初始化摄像头UI
camera_ui_t* camera_ui_init(lv_obj_t *display_img, const char *device, int width, int height, int fps);

// 反初始化摄像头UI
// 参数camera_ui: 摄像头UI实例，如果为NULL则使用全局实例
void camera_ui_deinit(camera_ui_t *camera_ui);

// 启动摄像头UI
// 参数camera_ui: 摄像头UI实例，如果为NULL则使用全局实例
int camera_ui_start(camera_ui_t *camera_ui);

// 停止摄像头UI
// 参数camera_ui: 摄像头UI实例，如果为NULL则使用全局实例
int camera_ui_stop(camera_ui_t *camera_ui);

// 获取摄像头状态
camera_ui_state_t camera_ui_get_state(camera_ui_t *camera_ui);

// 状态转字符串函数
const char* camera_ui_state_to_string(camera_ui_state_t state);

#endif