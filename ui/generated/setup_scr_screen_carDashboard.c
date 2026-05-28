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
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_carDashboard(lv_ui *ui)
{
    //Write codes screen_carDashboard
    ui->screen_carDashboard = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_carDashboard, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_carDashboard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_carBack
    ui->screen_carDashboard_img_carBack = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_carBack, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_carBack, &_musicBack2l1_alpha_800x480);
    lv_img_set_pivot(ui->screen_carDashboard_img_carBack, 0,0);
    lv_img_set_angle(ui->screen_carDashboard_img_carBack, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_carBack, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_carBack, 800, 480);

    //Write style for screen_carDashboard_img_carBack, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_carBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_carBack, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_carBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_carBack, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_base
    ui->screen_carDashboard_img_base = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_base, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_base, &_Base_alpha_800x400);
    lv_img_set_pivot(ui->screen_carDashboard_img_base, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_base, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_base, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_base, 800, 400);

    //Write style for screen_carDashboard_img_base, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_base, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_base, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_base, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_base, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_rSpeed
    ui->screen_carDashboard_cont_rSpeed = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_rSpeed, 520, 90);
    lv_obj_set_size(ui->screen_carDashboard_cont_rSpeed, 250, 250);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_rSpeed, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_rSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_rSpeed
    ui->screen_carDashboard_img_rSpeed = lv_img_create(ui->screen_carDashboard_cont_rSpeed);
    lv_obj_add_flag(ui->screen_carDashboard_img_rSpeed, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_rSpeed, &_gauge_bg_alpha_250x250);
    lv_img_set_pivot(ui->screen_carDashboard_img_rSpeed, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_rSpeed, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_rSpeed, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_rSpeed, 250, 250);

    //Write style for screen_carDashboard_img_rSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_rSpeed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_rSpeed, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_rSpeed, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_needle
    ui->screen_carDashboard_img_needle = lv_img_create(ui->screen_carDashboard_cont_rSpeed);
    lv_obj_add_flag(ui->screen_carDashboard_img_needle, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_needle, &_gauge_needle_alpha_94x12);
    lv_img_set_pivot(ui->screen_carDashboard_img_needle, -33,7);
    lv_img_set_angle(ui->screen_carDashboard_img_needle, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_needle, 158, 120);
    lv_obj_set_size(ui->screen_carDashboard_img_needle, 94, 12);
    lv_obj_add_flag(ui->screen_carDashboard_img_needle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_carDashboard_img_needle, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_carDashboard_img_needle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_needle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_needle, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_needle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_needle, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_carLog
    ui->screen_carDashboard_img_carLog = lv_img_create(ui->screen_carDashboard_cont_rSpeed);
    lv_obj_add_flag(ui->screen_carDashboard_img_carLog, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_carLog, &_car_icon_alpha_75x15);
    lv_img_set_pivot(ui->screen_carDashboard_img_carLog, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_carLog, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_carLog, 140, 165);
    lv_obj_set_size(ui->screen_carDashboard_img_carLog, 75, 15);

    //Write style for screen_carDashboard_img_carLog, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_carLog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_carLog, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_carLog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_carLog, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedNum1
    ui->screen_carDashboard_label_speedNum1 = lv_label_create(ui->screen_carDashboard_cont_rSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedNum1, "10000");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedNum1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedNum1, 140, 182);
    lv_obj_set_size(ui->screen_carDashboard_label_speedNum1, 75, 28);

    //Write style for screen_carDashboard_label_speedNum1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedNum1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedNum1, &lv_font_Acme_Regular_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedNum1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedNum1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedNum1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedNum2
    ui->screen_carDashboard_label_speedNum2 = lv_label_create(ui->screen_carDashboard_cont_rSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedNum2, "x1000r/min");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedNum2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedNum2, 88, 62);
    lv_obj_set_size(ui->screen_carDashboard_label_speedNum2, 75, 15);

    //Write style for screen_carDashboard_label_speedNum2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedNum2, lv_color_hex(0xa70f0f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedNum2, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedNum2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedNum2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedNum2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedRpm
    ui->screen_carDashboard_label_speedRpm = lv_label_create(ui->screen_carDashboard_cont_rSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedRpm, "rpm");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedRpm, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedRpm, 140, 205);
    lv_obj_set_size(ui->screen_carDashboard_label_speedRpm, 30, 15);

    //Write style for screen_carDashboard_label_speedRpm, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedRpm, lv_color_hex(0xa70f0f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedRpm, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedRpm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedRpm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedRpm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_meter_rSpeed
    ui->screen_carDashboard_meter_rSpeed = lv_meter_create(ui->screen_carDashboard_cont_rSpeed);
    // add scale ui->screen_carDashboard_meter_rSpeed_scale_0
    ui->screen_carDashboard_meter_rSpeed_scale_0 = lv_meter_add_scale(ui->screen_carDashboard_meter_rSpeed);
    lv_meter_set_scale_ticks(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0, 120, 0, 10, lv_color_hex(0xffffff));
    lv_meter_set_scale_range(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0, 0, 120, 300, 70);

    // add arc for ui->screen_carDashboard_meter_rSpeed_scale_0
    ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0 = lv_meter_add_arc(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0, 12, lv_color_hex(0xff0027), -5);
    lv_meter_set_indicator_start_value(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0, 8);
    lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0, 8);

    // add needle images for ui->screen_carDashboard_meter_rSpeed_scale_0.
    ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0 = lv_meter_add_needle_img(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0, &_gauge_needle_alpha_94x12, -33, 7);
    lv_meter_set_indicator_value(ui->screen_carDashboard_meter_rSpeed, ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0, 8);
    lv_obj_set_pos(ui->screen_carDashboard_meter_rSpeed, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_meter_rSpeed, 250, 250);

    //Write style for screen_carDashboard_meter_rSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_meter_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_meter_rSpeed, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_carDashboard_meter_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_meter_rSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_meter_rSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_meter_rSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_meter_rSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_meter_rSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_carDashboard_meter_rSpeed, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_carDashboard_meter_rSpeed, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_meter_rSpeed, &lv_font_SourceHanSerifSC_Regular_14, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_meter_rSpeed, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for screen_carDashboard_meter_rSpeed, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_meter_rSpeed, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_cSpeed
    ui->screen_carDashboard_cont_cSpeed = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_cSpeed, 30, 90);
    lv_obj_set_size(ui->screen_carDashboard_cont_cSpeed, 250, 250);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_cSpeed, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_cSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_cSpeed
    ui->screen_carDashboard_img_cSpeed = lv_img_create(ui->screen_carDashboard_cont_cSpeed);
    lv_obj_add_flag(ui->screen_carDashboard_img_cSpeed, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_cSpeed, &_gauge_bgL_alpha_250x250);
    lv_img_set_pivot(ui->screen_carDashboard_img_cSpeed, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_cSpeed, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_cSpeed, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_cSpeed, 250, 250);

    //Write style for screen_carDashboard_img_cSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_cSpeed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_cSpeed, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_cSpeed, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_carLog1
    ui->screen_carDashboard_img_carLog1 = lv_img_create(ui->screen_carDashboard_cont_cSpeed);
    lv_obj_add_flag(ui->screen_carDashboard_img_carLog1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_carLog1, &_car_icon_alpha_75x15);
    lv_img_set_pivot(ui->screen_carDashboard_img_carLog1, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_carLog1, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_carLog1, 140, 165);
    lv_obj_set_size(ui->screen_carDashboard_img_carLog1, 75, 15);

    //Write style for screen_carDashboard_img_carLog1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_carLog1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_carLog1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_carLog1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_carLog1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedNum4
    ui->screen_carDashboard_label_speedNum4 = lv_label_create(ui->screen_carDashboard_cont_cSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedNum4, "200");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedNum4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedNum4, 140, 182);
    lv_obj_set_size(ui->screen_carDashboard_label_speedNum4, 50, 28);

    //Write style for screen_carDashboard_label_speedNum4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedNum4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedNum4, &lv_font_Acme_Regular_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedNum4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedNum4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedNum4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedNum3
    ui->screen_carDashboard_label_speedNum3 = lv_label_create(ui->screen_carDashboard_cont_cSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedNum3, "x1km/h");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedNum3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedNum3, 86, 62);
    lv_obj_set_size(ui->screen_carDashboard_label_speedNum3, 75, 15);

    //Write style for screen_carDashboard_label_speedNum3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedNum3, lv_color_hex(0xa70f0f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedNum3, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedNum3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedNum3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedNum3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_speedSpd
    ui->screen_carDashboard_label_speedSpd = lv_label_create(ui->screen_carDashboard_cont_cSpeed);
    lv_label_set_text(ui->screen_carDashboard_label_speedSpd, "spd");
    lv_label_set_long_mode(ui->screen_carDashboard_label_speedSpd, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_speedSpd, 140, 205);
    lv_obj_set_size(ui->screen_carDashboard_label_speedSpd, 30, 13);

    //Write style for screen_carDashboard_label_speedSpd, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_speedSpd, lv_color_hex(0xa70f0f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_speedSpd, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_speedSpd, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_speedSpd, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_speedSpd, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_meter_cSpeed
    ui->screen_carDashboard_meter_cSpeed = lv_meter_create(ui->screen_carDashboard_cont_cSpeed);
    // add scale ui->screen_carDashboard_meter_cSpeed_scale_0
    ui->screen_carDashboard_meter_cSpeed_scale_0 = lv_meter_add_scale(ui->screen_carDashboard_meter_cSpeed);
    lv_meter_set_scale_ticks(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0, 120, 0, 10, lv_color_hex(0xffffff));
    lv_meter_set_scale_range(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0, 0, 120, 300, 70);

    // add arc for ui->screen_carDashboard_meter_cSpeed_scale_0
    ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0 = lv_meter_add_arc(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0, 12, lv_color_hex(0xff0027), -5);
    lv_meter_set_indicator_start_value(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0, 8);
    lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0, 8);

    // add needle images for ui->screen_carDashboard_meter_cSpeed_scale_0.
    ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0 = lv_meter_add_needle_img(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0, &_gauge_needle_alpha_94x12, -33, 7);
    lv_meter_set_indicator_value(ui->screen_carDashboard_meter_cSpeed, ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0, 8);
    lv_obj_set_pos(ui->screen_carDashboard_meter_cSpeed, 0, 0);
    lv_obj_set_size(ui->screen_carDashboard_meter_cSpeed, 250, 250);

    //Write style for screen_carDashboard_meter_cSpeed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_meter_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_meter_cSpeed, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_carDashboard_meter_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_meter_cSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_meter_cSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_meter_cSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_meter_cSpeed, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_meter_cSpeed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_carDashboard_meter_cSpeed, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_carDashboard_meter_cSpeed, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_meter_cSpeed, &lv_font_SourceHanSerifSC_Regular_14, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_meter_cSpeed, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for screen_carDashboard_meter_cSpeed, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_meter_cSpeed, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_line1
    ui->screen_carDashboard_img_line1 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_line1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_line1, &_line1_alpha_141x266);
    lv_img_set_pivot(ui->screen_carDashboard_img_line1, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_line1, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_line1, 434, 115);
    lv_obj_set_size(ui->screen_carDashboard_img_line1, 141, 266);

    //Write style for screen_carDashboard_img_line1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_line1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_line1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_line1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_line1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_line2
    ui->screen_carDashboard_img_line2 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_line2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_line2, &_line2_alpha_141x266);
    lv_img_set_pivot(ui->screen_carDashboard_img_line2, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_line2, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_line2, 225, 115);
    lv_obj_set_size(ui->screen_carDashboard_img_line2, 141, 266);

    //Write style for screen_carDashboard_img_line2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_line2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_line2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_line2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_line2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_line3
    ui->screen_carDashboard_img_line3 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_line3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_line3, &_line3_alpha_183x340);
    lv_img_set_pivot(ui->screen_carDashboard_img_line3, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_line3, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_line3, 427, 80);
    lv_obj_set_size(ui->screen_carDashboard_img_line3, 183, 340);

    //Write style for screen_carDashboard_img_line3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_line3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_line3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_line3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_line3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_line4
    ui->screen_carDashboard_img_line4 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_line4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_line4, &_line4_alpha_183x340);
    lv_img_set_pivot(ui->screen_carDashboard_img_line4, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_line4, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_line4, 190, 80);
    lv_obj_set_size(ui->screen_carDashboard_img_line4, 183, 340);

    //Write style for screen_carDashboard_img_line4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_line4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_line4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_line4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_line4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_cen1
    ui->screen_carDashboard_img_cen1 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_cen1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_cen1, &_cen1_alpha_56x300);
    lv_img_set_pivot(ui->screen_carDashboard_img_cen1, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_cen1, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_cen1, 372, 100);
    lv_obj_set_size(ui->screen_carDashboard_img_cen1, 56, 300);

    //Write style for screen_carDashboard_img_cen1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_cen1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_cen1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_cen1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_cen1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_largeNum
    ui->screen_carDashboard_label_largeNum = lv_label_create(ui->screen_carDashboard);
    lv_label_set_text(ui->screen_carDashboard_label_largeNum, "200");
    lv_label_set_long_mode(ui->screen_carDashboard_label_largeNum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_largeNum, 280, 35);
    lv_obj_set_size(ui->screen_carDashboard_label_largeNum, 240, 145);

    //Write style for screen_carDashboard_label_largeNum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_largeNum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_largeNum, &lv_font_Acme_Regular_145, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_largeNum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_largeNum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_largeNum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_label_largeU
    ui->screen_carDashboard_label_largeU = lv_label_create(ui->screen_carDashboard);
    lv_label_set_text(ui->screen_carDashboard_label_largeU, "km/h");
    lv_label_set_long_mode(ui->screen_carDashboard_label_largeU, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_carDashboard_label_largeU, 280, 165);
    lv_obj_set_size(ui->screen_carDashboard_label_largeU, 240, 40);

    //Write style for screen_carDashboard_label_largeU, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_carDashboard_label_largeU, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_carDashboard_label_largeU, &lv_font_Acme_Regular_35, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_carDashboard_label_largeU, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_carDashboard_label_largeU, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_label_largeU, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_arc1
    ui->screen_carDashboard_img_arc1 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_arc1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_arc1, &_arc1_alpha_362x17);
    lv_img_set_pivot(ui->screen_carDashboard_img_arc1, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_arc1, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_arc1, 219, 395);
    lv_obj_set_size(ui->screen_carDashboard_img_arc1, 362, 17);

    //Write style for screen_carDashboard_img_arc1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_arc1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_arc1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_arc1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_arc1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_arc2
    ui->screen_carDashboard_img_arc2 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_arc2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_arc2, &_arc2_alpha_362x17);
    lv_img_set_pivot(ui->screen_carDashboard_img_arc2, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_arc2, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_arc2, 219, 35);
    lv_obj_set_size(ui->screen_carDashboard_img_arc2, 362, 17);

    //Write style for screen_carDashboard_img_arc2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_arc2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_arc2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_arc2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_arc2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_bt
    ui->screen_carDashboard_cont_bt = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_bt, 199, 385);
    lv_obj_set_size(ui->screen_carDashboard_cont_bt, 400, 70);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_bt, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_bt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_bt, 30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_bt, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_carDashboard_cont_bt, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_carDashboard_cont_bt, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_tpR
    ui->screen_carDashboard_cont_tpR = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_tpR, 570, 35);
    lv_obj_set_size(ui->screen_carDashboard_cont_tpR, 170, 50);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_tpR, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_tpR, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_tpR, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_tpR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_AbsOff
    ui->screen_carDashboard_img_AbsOff = lv_img_create(ui->screen_carDashboard_cont_tpR);
    lv_obj_add_flag(ui->screen_carDashboard_img_AbsOff, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_AbsOff, &_ABS0_alpha_55x45);
    lv_img_set_pivot(ui->screen_carDashboard_img_AbsOff, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_AbsOff, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_AbsOff, 5, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_AbsOff, 55, 45);

    //Write style for screen_carDashboard_img_AbsOff, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_AbsOff, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_AbsOff, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_AbsOff, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_AbsOff, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_Door
    ui->screen_carDashboard_img_Door = lv_img_create(ui->screen_carDashboard_cont_tpR);
    lv_obj_add_flag(ui->screen_carDashboard_img_Door, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_Door, &_Door_alpha_45x45);
    lv_img_set_pivot(ui->screen_carDashboard_img_Door, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_Door, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_Door, 67, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_Door, 45, 45);

    //Write style for screen_carDashboard_img_Door, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_Door, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_Door, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_Door, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_Door, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_Seatbelt
    ui->screen_carDashboard_img_Seatbelt = lv_img_create(ui->screen_carDashboard_cont_tpR);
    lv_obj_add_flag(ui->screen_carDashboard_img_Seatbelt, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_Seatbelt, &_Seatbelt_alpha_45x45);
    lv_img_set_pivot(ui->screen_carDashboard_img_Seatbelt, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_Seatbelt, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_Seatbelt, 120, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_Seatbelt, 45, 45);

    //Write style for screen_carDashboard_img_Seatbelt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_Seatbelt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_Seatbelt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_Seatbelt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_Seatbelt, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_AbsOn
    ui->screen_carDashboard_img_AbsOn = lv_img_create(ui->screen_carDashboard_cont_tpR);
    lv_obj_add_flag(ui->screen_carDashboard_img_AbsOn, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_AbsOn, &_ABS1_alpha_55x45);
    lv_img_set_pivot(ui->screen_carDashboard_img_AbsOn, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_AbsOn, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_AbsOn, 5, 0);
    lv_obj_set_size(ui->screen_carDashboard_img_AbsOn, 55, 45);
    lv_obj_add_flag(ui->screen_carDashboard_img_AbsOn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_carDashboard_img_AbsOn, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_carDashboard_img_AbsOn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_AbsOn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_AbsOn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_AbsOn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_AbsOn, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_btL
    ui->screen_carDashboard_cont_btL = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_btL, 45, 395);
    lv_obj_set_size(ui->screen_carDashboard_cont_btL, 120, 50);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_btL, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_btL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_btL, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_btL, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_carDashboard_cont_btL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_carDashboard_cont_btL, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_btL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_ACC
    ui->screen_carDashboard_img_ACC = lv_img_create(ui->screen_carDashboard_cont_btL);
    lv_obj_add_flag(ui->screen_carDashboard_img_ACC, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_ACC, &_ACC_alpha_45x40);
    lv_img_set_pivot(ui->screen_carDashboard_img_ACC, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_ACC, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_ACC, 65, 5);
    lv_obj_set_size(ui->screen_carDashboard_img_ACC, 45, 40);

    //Write style for screen_carDashboard_img_ACC, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_ACC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_ACC, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_ACC, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_ACC, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_HighBeam
    ui->screen_carDashboard_img_HighBeam = lv_img_create(ui->screen_carDashboard_cont_btL);
    lv_obj_add_flag(ui->screen_carDashboard_img_HighBeam, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_HighBeam, &_HighBeam_alpha_42x35);
    lv_img_set_pivot(ui->screen_carDashboard_img_HighBeam, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_HighBeam, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_HighBeam, 10, 8);
    lv_obj_set_size(ui->screen_carDashboard_img_HighBeam, 42, 35);

    //Write style for screen_carDashboard_img_HighBeam, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_HighBeam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_HighBeam, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_HighBeam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_HighBeam, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_btR
    ui->screen_carDashboard_cont_btR = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_btR, 635, 395);
    lv_obj_set_size(ui->screen_carDashboard_cont_btR, 120, 50);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_btR, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_btR, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_btR, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_btR, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_carDashboard_cont_btR, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_carDashboard_cont_btR, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_btR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_RearFog
    ui->screen_carDashboard_img_RearFog = lv_img_create(ui->screen_carDashboard_cont_btR);
    lv_obj_add_flag(ui->screen_carDashboard_img_RearFog, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_RearFog, &_RearFog_alpha_45x40);
    lv_img_set_pivot(ui->screen_carDashboard_img_RearFog, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_RearFog, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_RearFog, 65, 5);
    lv_obj_set_size(ui->screen_carDashboard_img_RearFog, 45, 40);

    //Write style for screen_carDashboard_img_RearFog, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_RearFog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_RearFog, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_RearFog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_RearFog, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_FrontFog
    ui->screen_carDashboard_img_FrontFog = lv_img_create(ui->screen_carDashboard_cont_btR);
    lv_obj_add_flag(ui->screen_carDashboard_img_FrontFog, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_FrontFog, &_FrontFog_alpha_45x40);
    lv_img_set_pivot(ui->screen_carDashboard_img_FrontFog, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_FrontFog, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_FrontFog, 10, 5);
    lv_obj_set_size(ui->screen_carDashboard_img_FrontFog, 45, 40);

    //Write style for screen_carDashboard_img_FrontFog, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_FrontFog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_FrontFog, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_FrontFog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_FrontFog, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_cont_tpL
    ui->screen_carDashboard_cont_tpL = lv_obj_create(ui->screen_carDashboard);
    lv_obj_set_pos(ui->screen_carDashboard_cont_tpL, 90, 35);
    lv_obj_set_size(ui->screen_carDashboard_cont_tpL, 120, 50);
    lv_obj_set_scrollbar_mode(ui->screen_carDashboard_cont_tpL, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_carDashboard_cont_tpL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_cont_tpL, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_carDashboard_cont_tpL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_turnL
    ui->screen_carDashboard_img_turnL = lv_img_create(ui->screen_carDashboard_cont_tpL);
    lv_obj_add_flag(ui->screen_carDashboard_img_turnL, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_turnL, &_TurnSignalL_alpha_60x55);
    lv_img_set_pivot(ui->screen_carDashboard_img_turnL, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_turnL, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_turnL, 1, -2);
    lv_obj_set_size(ui->screen_carDashboard_img_turnL, 60, 55);

    //Write style for screen_carDashboard_img_turnL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_turnL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_turnL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_turnL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_turnL, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_turnR
    ui->screen_carDashboard_img_turnR = lv_img_create(ui->screen_carDashboard_cont_tpL);
    lv_obj_add_flag(ui->screen_carDashboard_img_turnR, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_turnR, &_TurnSignalR_alpha_60x55);
    lv_img_set_pivot(ui->screen_carDashboard_img_turnR, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_turnR, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_turnR, 59, -2);
    lv_obj_set_size(ui->screen_carDashboard_img_turnR, 60, 55);

    //Write style for screen_carDashboard_img_turnR, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_turnR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_turnR, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_turnR, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_turnR, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_carDashboard_img_car1
    ui->screen_carDashboard_img_car1 = lv_img_create(ui->screen_carDashboard);
    lv_obj_add_flag(ui->screen_carDashboard_img_car1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_carDashboard_img_car1, &_car1_alpha_100x100);
    lv_img_set_pivot(ui->screen_carDashboard_img_car1, 50,50);
    lv_img_set_angle(ui->screen_carDashboard_img_car1, 0);
    lv_obj_set_pos(ui->screen_carDashboard_img_car1, 350, 235);
    lv_obj_set_size(ui->screen_carDashboard_img_car1, 100, 100);

    //Write style for screen_carDashboard_img_car1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_carDashboard_img_car1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_carDashboard_img_car1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_carDashboard_img_car1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_carDashboard_img_car1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_carDashboard.
    //img_arc


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_carDashboard);

    //Init events for screen.
    events_init_screen_carDashboard(ui);
}
