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
#include "custom.h"
#include "custom_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
static lv_obj_t *ctrl_center = NULL;
static lv_obj_t *status_bar = NULL;
static lv_timer_t *digital_clock_timer = NULL;
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
    custom_font_init("/home/debian/font/SourceHanSerifSC-Regular.otf", 16);

    ctrl_center = ui->screen_home_cont_control_center;
    //移到系统顶层
    lv_obj_set_parent(ctrl_center, lv_layer_top());

    status_bar = ui->screen_home_cont_status_bar;
    //移到系统顶层
    lv_obj_set_parent(status_bar, lv_layer_top()); 
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_CLICKABLE);
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

bool get_ctrl_center_state()
{
   if(ctrl_center != NULL) {
        return true;
   }
   
   return false;
}

bool get_status_bar_state()
{
   if(status_bar != NULL) {
        return true;
   }
   
   return false;
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