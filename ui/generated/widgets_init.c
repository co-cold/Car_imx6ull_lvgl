/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
#if LV_USE_KEYBOARD || LV_USE_ZH_KEYBOARD
    lv_obj_t *ta = lv_event_get_target(e);
#endif
    lv_obj_t *kb = lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED)
    {
#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_foreground(kb);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED)
    {

#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_background(kb);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

#if LV_USE_ANALOGCLOCK != 0
void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}
#endif

/* 有bug弃用 guider_ui的成员会成为悬空指针，全局定时器会导致程序崩溃 */

/* 全局时钟定时器 */
// void screen_home_digital_clock_status_timer(lv_timer_t *timer) 
// { 
//     time_t now = time(NULL); 
//     struct tm *timeinfo = localtime(&now); 
     
//     if (lv_obj_is_valid(guider_ui.screen_home_digital_clock_status)) 
//     { 
//         // 24小时制 
//         lv_dclock_set_text_fmt(guider_ui.screen_home_digital_clock_status,
//                               "%02d:%02d:%02d", 
//                               timeinfo->tm_hour, 
//                               timeinfo->tm_min, 
//                               timeinfo->tm_sec); 
//     } 
// } 

// void screen_clock_analog_clock_1_timer(lv_timer_t *timer)
// {
//     time_t now = time(NULL); 
//     struct tm *timeinfo = localtime(&now); 

//     int32_t hour_12 = timeinfo->tm_hour % 12;  // 直接使用 0-11 的小时值 
//     if (lv_obj_is_valid(guider_ui.screen_clock_analog_clock_1))
//     {
//         lv_analogclock_set_time(guider_ui.screen_clock_analog_clock_1, 
//                                 hour_12, 
//                                 timeinfo->tm_min, 
//                                 timeinfo->tm_sec); 
//     }
// }

// void screen_clock_digital_clock_1_timer(lv_timer_t *timer)
// {
//     time_t now = time(NULL); 
//     struct tm *timeinfo = localtime(&now); 
     
//     if (lv_obj_is_valid(guider_ui.screen_clock_digital_clock_1)) 
//     { 
//         // 24小时制 
//         lv_dclock_set_text_fmt(guider_ui.screen_clock_digital_clock_1,
//                               "%02d:%02d:%02d", 
//                               timeinfo->tm_hour, 
//                               timeinfo->tm_min, 
//                               timeinfo->tm_sec); 
//     } 
// }

// //Write codes screen_home_digital_clock_status
// static bool screen_home_digital_clock_status_timer_enabled = false;
// // // 获取当前系统时间
// time_t now = time(NULL);
// struct tm *timeinfo = localtime(&now);
// char time_str[20];
// snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
//             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

// // 使用当前时间作为初始值
// ui->screen_home_digital_clock_status = lv_dclock_create(ui->screen_home_cont_status_bar, time_str);

// // 同时更新全局变量
// screen_home_digital_clock_status_hour_value = timeinfo->tm_hour;
// screen_home_digital_clock_status_min_value = timeinfo->tm_min;
// screen_home_digital_clock_status_sec_value = timeinfo->tm_sec;
// // 获取当前系统时间
// if (!screen_home_digital_clock_status_timer_enabled) {
//     lv_timer_create(screen_home_digital_clock_status_timer, 1000, NULL);
//     screen_home_digital_clock_status_timer_enabled = true;
// }

// // 获取当前系统时间
// time_t now = time(NULL);
// struct tm *timeinfo = localtime(&now);
// int32_t hour_12 = timeinfo->tm_hour % 12; 
// lv_analogclock_set_time(ui->screen_clock_analog_clock_1, 
//                         hour_12, 
//                         timeinfo->tm_min,
//                         timeinfo->tm_sec);

// //Write codes screen_clock_digital_clock_1
// static bool screen_clock_digital_clock_1_timer_enabled = false;
// // 获取当前系统时间
// now = time(NULL);
// timeinfo = localtime(&now);
// char time_str[20];
// snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
//             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
// ui->screen_clock_digital_clock_1 = lv_dclock_create(ui->screen_clock_cont_clock, time_str);
// if (!screen_clock_digital_clock_1_timer_enabled) {
//     lv_timer_create(screen_clock_digital_clock_1_timer, 1000, NULL);
//     screen_clock_digital_clock_1_timer_enabled = true;
// }