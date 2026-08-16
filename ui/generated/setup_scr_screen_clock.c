/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include <time.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"





void setup_scr_screen_clock(lv_ui *ui)
{
    //Write codes screen_clock
    ui->screen_clock = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_clock, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_clock, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_clock_cont_clock
    ui->screen_clock_cont_clock = lv_obj_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_cont_clock, 0, 0);
    lv_obj_set_size(ui->screen_clock_cont_clock, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_clock_cont_clock, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_clock_cont_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_clock_cont_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_cont_clock, lv_color_hex(0xadadad), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_cont_clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_cont_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_clock_analog_clock_1
    static bool screen_clock_analog_clock_1_timer_enabled = false;
    ui->screen_clock_analog_clock_1 = lv_analogclock_create(ui->screen_clock_cont_clock);
    lv_analogclock_hide_digits(ui->screen_clock_analog_clock_1, false);
    lv_analogclock_set_major_ticks(ui->screen_clock_analog_clock_1, 5, 10, lv_color_hex(0x555555), 10);
    lv_analogclock_set_ticks(ui->screen_clock_analog_clock_1, 5, 5, lv_color_hex(0x333333));
    lv_analogclock_set_hour_needle_line(ui->screen_clock_analog_clock_1, 5, lv_color_hex(0x000000), -50);
    lv_analogclock_set_min_needle_line(ui->screen_clock_analog_clock_1, 4, lv_color_hex(0x000000), -30);
    lv_analogclock_set_sec_needle_line(ui->screen_clock_analog_clock_1, 3, lv_color_hex(0x000000), -10);

    /* 修复样式 */
    lv_obj_set_style_radius(ui->screen_clock_analog_clock_1, LV_RADIUS_CIRCLE, LV_PART_MAIN|LV_STATE_DEFAULT);
    static lv_style_t screen_clock_analog_clock_1_style;
    lv_style_init(&screen_clock_analog_clock_1_style);           //初始化样式
    lv_style_set_pad_top(&screen_clock_analog_clock_1_style, 5);
    lv_style_set_pad_left(&screen_clock_analog_clock_1_style, 5);
    lv_style_set_pad_right(&screen_clock_analog_clock_1_style, 5);
    lv_style_set_pad_bottom(&screen_clock_analog_clock_1_style, 5);
    lv_obj_add_style(ui->screen_clock_analog_clock_1, &screen_clock_analog_clock_1_style, LV_PART_MAIN);
    
    // 获取当前系统时间
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    int32_t hour_12 = timeinfo->tm_hour % 12;  // 直接使用 0-11 的小时值 
    lv_analogclock_set_time(guider_ui.screen_clock_analog_clock_1, 
                        hour_12, 
                        timeinfo->tm_min, 
                        timeinfo->tm_sec); 
    // create timer
    if (!screen_clock_analog_clock_1_timer_enabled) {
        // lv_timer_create(screen_clock_analog_clock_1_timer, 1000, NULL);
        screen_clock_analog_clock_1_timer_enabled = true;
    }
    lv_obj_set_pos(ui->screen_clock_analog_clock_1, 40, 40);
    lv_obj_set_size(ui->screen_clock_analog_clock_1, 300, 300);

    //Write style for screen_clock_analog_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_analog_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_analog_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_analog_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_analog_clock_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_clock_analog_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_clock_analog_clock_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_clock_analog_clock_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_clock_analog_clock_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_clock_analog_clock_1, lv_color_hex(0x000000), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_clock_analog_clock_1, &lv_font_SourceHanSerifSC_Regular_24, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_clock_analog_clock_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for screen_clock_analog_clock_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_analog_clock_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_analog_clock_1, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_analog_clock_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_clock_digital_clock_1
    static bool screen_clock_digital_clock_1_timer_enabled = false;
    // 获取当前系统时间
    now = time(NULL);
    timeinfo = localtime(&now);
    char time_str[20];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
                timeinfo->tm_hour, 
                timeinfo->tm_min, 
                timeinfo->tm_sec);
    ui->screen_clock_digital_clock_1 = lv_dclock_create(ui->screen_clock_cont_clock, time_str);
    if (!screen_clock_digital_clock_1_timer_enabled) {
        // lv_timer_create(screen_clock_digital_clock_1_timer, 1000, NULL);
        screen_clock_digital_clock_1_timer_enabled = true;
    }
    lv_obj_set_pos(ui->screen_clock_digital_clock_1, 40, 380);
    lv_obj_set_size(ui->screen_clock_digital_clock_1, 300, 50);

    //Write style for screen_clock_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_clock_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_clock_digital_clock_1, &lv_font_SourceHanSerifSC_Regular_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_clock_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_clock_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_clock_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_clock.
    lv_obj_clear_flag(ui->screen_clock, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_clock_cont_clock, LV_OBJ_FLAG_SCROLLABLE);


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_clock);

    //Init events for screen.
    events_init_screen_clock(ui);
}