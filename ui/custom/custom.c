/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom_mem_trace.h"
#include "custom.h"
#include "custom_font.h"

/*********************
 *      DEFINES
 *********************/
#include <time.h>
/**********************
 *      TYPEDEFS
 **********************/
static lv_obj_t *ctrl_center = NULL;
static lv_obj_t *status_bar = NULL;
static lv_obj_t *Gclock = NULL;
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

static void mem_leak_check_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    mem_trace_stats();
}

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
    custom_font_init("/home/debian/font/SourceHanSerifSC-Regular.otf", 16);

    lv_timer_create(mem_leak_check_timer_cb, 30000, NULL);
}

void custom_top_init(lv_ui *ui)
{
    /* 1. 处理控制栏 */
    if (ctrl_center == NULL) {
        // 第一次进入：保存新创建的对象
        ctrl_center = ui->screen_home_cont_control_center;
    } else {
        if (lv_obj_is_valid(ctrl_center)) {
            // 非第一次：隐藏新创建的对象，显示旧对象
            lv_obj_add_flag(ui->screen_home_cont_control_center, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ctrl_center, LV_OBJ_FLAG_HIDDEN);
            
            // ✅ 关键修复：删除新创建的对象，避免内存泄漏
            if (lv_obj_is_valid(ui->screen_home_cont_control_center)) {
                lv_obj_del(ui->screen_home_cont_control_center);
            }
            
            // 同步 ui 指针到全局对象
            ui->screen_home_cont_control_center = ctrl_center;
        } else {
            // 旧对象已失效，重新初始化
            ctrl_center = ui->screen_home_cont_control_center;
        }
    }
    lv_obj_set_parent(ctrl_center, lv_layer_top());

    /* 2. 处理状态栏 + 时钟（关键同步） */
    if (status_bar == NULL) {
        // 第一次进入：保存状态栏和时钟
        status_bar = ui->screen_home_cont_status_bar;
        Gclock = ui->screen_home_digital_clock_status;
    } else {
        if (lv_obj_is_valid(status_bar) && lv_obj_is_valid(Gclock)) {
            // 非第一次：隐藏新创建的状态栏，显示旧状态栏
            lv_obj_add_flag(ui->screen_home_cont_status_bar, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
            
            // ✅ 关键修复：删除新创建的状态栏（含子时钟）
            if (lv_obj_is_valid(ui->screen_home_cont_status_bar)) {
                lv_obj_del(ui->screen_home_cont_status_bar);
            }
            
            // 同步 ui 指针到全局对象
            ui->screen_home_cont_status_bar = status_bar;
            ui->screen_home_digital_clock_status = Gclock;
        } else {
            // 旧对象已失效，重新初始化
            status_bar = ui->screen_home_cont_status_bar;
            Gclock = ui->screen_home_digital_clock_status;
        }
    }
    
    // 确保全局对象在顶层
    lv_obj_set_parent(status_bar, lv_layer_top());
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
    
    // 确保时钟依附于状态栏（双重保险）
    if (lv_obj_is_valid(Gclock) && lv_obj_is_valid(status_bar)) {
        lv_obj_set_parent(Gclock, status_bar);
    }

    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_CLICKABLE);
    custom_set_Htimer();
}

void show_control_center()
{
    if (!ctrl_center) return;
    
    lv_obj_clear_flag(ctrl_center, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(ctrl_center);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ctrl_center);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, -480, 0);
    lv_anim_set_time(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void hide_control_center()
{
    if (!ctrl_center) return;
  
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ctrl_center);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, 0, -480);
    lv_anim_set_time(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
}

void set_status_bar(uint8_t state)
{
    if(status_bar == NULL){return;}
    if(state == 1) {
        lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
    } else if(state == 0) {
        lv_obj_add_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
    }
}

void slider_set_lable_light(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider) return;
    
    int32_t value = lv_slider_get_value(slider);
    
    // 获取滑块的关联标签
    lv_obj_t *label = lv_obj_get_user_data(slider);
    if (label) {
        lv_label_set_text_fmt(label, "%d%%", value);
    }
}

float slider_update_volume_label(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider) return 0.0f;
    
    // 获取滑块值并更新标签
    int32_t value = lv_slider_get_value(slider);
    lv_obj_t *label = lv_obj_get_user_data(slider);
    if (label) {
        lv_label_set_text_fmt(label, "%d%%", value);
    }
    
    // 转换为音量值 (0.0 - 1.0)，滑块值直接对应百分比
    float volume = value / 100.0f;
    return volume;
}



/**
  自定义时钟定时器
*/
static lv_timer_t *status_bar_timer = NULL;  // 状态栏时钟定时器（常驻）
static lv_timer_t *screen_clock_timer = NULL; // 屏幕时钟定时器（随屏幕销毁）
static lv_obj_t *Gclock_analog = NULL;
static lv_obj_t *Gclock_digital = NULL;

void ALL_clock_timer_cb(lv_timer_t *timer)
{
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);

    /* 更新状态栏时钟（常驻） */
    if (Gclock && lv_obj_is_valid(Gclock)) {
        lv_dclock_set_text_fmt(Gclock, "%02d:%02d:%02d",
                              timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }

    /* 更新屏幕模拟时钟 */
    if (Gclock_analog && lv_obj_is_valid(Gclock_analog)) {
        int32_t hour_12 = timeinfo->tm_hour % 12;
        lv_analogclock_set_time(Gclock_analog, hour_12, timeinfo->tm_min, timeinfo->tm_sec);
    }

    /* 更新屏幕数字时钟 */
    if (Gclock_digital && lv_obj_is_valid(Gclock_digital)) {
        lv_dclock_set_text_fmt(Gclock_digital, "%02d:%02d:%02d",
                              timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
}

void custom_set_Htimer()
{
    if(status_bar_timer == NULL) {
        status_bar_timer = lv_timer_create(ALL_clock_timer_cb, 1000, NULL);
    } 
}

void custom_set_Ctimer()
{
    if(screen_clock_timer == NULL) {
        screen_clock_timer = lv_timer_create(ALL_clock_timer_cb, 1000, NULL);
    } 
}

void custom_close_Htimer()
{
    if(status_bar_timer) {
        lv_timer_del(status_bar_timer);
        status_bar_timer = NULL;
    }
}

void custom_close_Ctimer()
{
    if(screen_clock_timer) {
        lv_timer_del(screen_clock_timer);
        screen_clock_timer = NULL;
    }
    Gclock_analog = NULL;
    Gclock_digital = NULL;
}

void custom_set_Aclock(lv_obj_t *clock)
{
    Gclock_analog = clock;
}

void custom_set_Dclock(lv_obj_t *clock)
{
    Gclock_digital = clock;
}

void custom_reset_clock_ptrs(void)
{
    Gclock = NULL;
    Gclock_analog = NULL;
    Gclock_digital = NULL;
}