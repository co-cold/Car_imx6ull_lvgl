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




void setup_scr_screen_home(lv_ui *ui)
{
    //Write codes screen_home
    ui->screen_home = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_home, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_home, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_cont_control_center
    ui->screen_home_cont_control_center = lv_obj_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_cont_control_center, 0, -480);
    lv_obj_set_size(ui->screen_home_cont_control_center, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_home_cont_control_center, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_home_cont_control_center, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_home_cont_control_center, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_cont_control_center, 181, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_cont_control_center, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_cont_control_center, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_cont_control_center, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_slider_volume
    ui->screen_home_slider_volume = lv_slider_create(ui->screen_home_cont_control_center);
    lv_slider_set_range(ui->screen_home_slider_volume, 0, 100);
    lv_slider_set_mode(ui->screen_home_slider_volume, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_home_slider_volume, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_home_slider_volume, 671, 150);
    lv_obj_set_size(ui->screen_home_slider_volume, 60, 230);

    //Write style for screen_home_slider_volume, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_volume, 66, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_slider_volume, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_volume, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_volume, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_home_slider_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_slider_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_slider_volume, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_volume, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_slider_volume, lv_color_hex(0xffffff), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_volume, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_volume, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_home_slider_volume, Part: LV_PART_INDICATOR, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_volume, 255, LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_home_slider_volume, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_volume, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_home_slider_volume, 8, LV_PART_INDICATOR|LV_STATE_DISABLED);

    //Write style for screen_home_slider_volume, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_volume, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_volume, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_home_slider_light
    ui->screen_home_slider_light = lv_slider_create(ui->screen_home_cont_control_center);
    lv_slider_set_range(ui->screen_home_slider_light, 0, 100);
    lv_slider_set_mode(ui->screen_home_slider_light, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_home_slider_light, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_home_slider_light, 580, 150);
    lv_obj_set_size(ui->screen_home_slider_light, 60, 230);

    //Write style for screen_home_slider_light, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_light, 66, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_slider_light, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_light, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_light, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_home_slider_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_slider_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_slider_light, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_light, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_slider_light, lv_color_hex(0xffffff), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_light, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_light, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_home_slider_light, Part: LV_PART_INDICATOR, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_light, 255, LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_home_slider_light, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_slider_light, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_home_slider_light, 8, LV_PART_INDICATOR|LV_STATE_DISABLED);

    //Write style for screen_home_slider_light, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_slider_light, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_slider_light, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_home_label_volume
    ui->screen_home_label_volume = lv_label_create(ui->screen_home_cont_control_center);
    lv_label_set_text(ui->screen_home_label_volume, "50%");
    lv_label_set_long_mode(ui->screen_home_label_volume, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_home_label_volume, 674, 344);
    lv_obj_set_size(ui->screen_home_label_volume, 50, 30);

    //Write style for screen_home_label_volume, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_volume, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_volume, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_volume, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_volume, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_light
    ui->screen_home_label_light = lv_label_create(ui->screen_home_cont_control_center);
    lv_label_set_text(ui->screen_home_label_light, "50%");
    lv_label_set_long_mode(ui->screen_home_label_light, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_home_label_light, 583, 344);
    lv_obj_set_size(ui->screen_home_label_light, 50, 30);

    //Write style for screen_home_label_light, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_light, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_light, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_light, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_light, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_settin
    ui->screen_home_btn_settin = lv_btn_create(ui->screen_home_cont_control_center);
    ui->screen_home_btn_settin_label = lv_label_create(ui->screen_home_btn_settin);
    lv_label_set_text(ui->screen_home_btn_settin_label, " " LV_SYMBOL_SETTINGS " ");
    lv_label_set_long_mode(ui->screen_home_btn_settin_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_settin_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_settin, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_settin_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_settin, 580, 50);
    lv_obj_set_size(ui->screen_home_btn_settin, 150, 70);

    //Write style for screen_home_btn_settin, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_settin, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_btn_settin, lv_color_hex(0xa5a5a5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_btn_settin, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_settin, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_settin, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_settin, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_settin, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_settin, &lv_font_SourceHanSerifSC_Regular_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_settin, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_settin, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_cont_touch
    ui->screen_home_cont_touch = lv_obj_create(ui->screen_home_cont_control_center);
    lv_obj_set_pos(ui->screen_home_cont_touch, 0, 420);
    lv_obj_set_size(ui->screen_home_cont_touch, 800, 60);
    lv_obj_set_scrollbar_mode(ui->screen_home_cont_touch, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_home_cont_touch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_cont_touch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_cont_home
    ui->screen_home_cont_home = lv_obj_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_cont_home, 0, 0);
    lv_obj_set_size(ui->screen_home_cont_home, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_home_cont_home, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_home_cont_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_cont_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_cont_home, lv_color_hex(0x070707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_cont_home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_cont_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_img_background
    ui->screen_home_img_background = lv_img_create(ui->screen_home_cont_home);
    lv_obj_add_flag(ui->screen_home_img_background, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_home_img_background, &_1694234191268_alpha_800x480);
    lv_img_set_pivot(ui->screen_home_img_background, 50,50);
    lv_img_set_angle(ui->screen_home_img_background, 0);
    lv_obj_set_pos(ui->screen_home_img_background, 0, 0);
    lv_obj_set_size(ui->screen_home_img_background, 800, 480);

    //Write style for screen_home_img_background, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_home_img_background, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_home_img_background, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_img_background, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_home_img_background, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_tileview_app
    ui->screen_home_tileview_app = lv_tileview_create(ui->screen_home_cont_home);
    ui->screen_home_tileview_app_tile1 = lv_tileview_add_tile(ui->screen_home_tileview_app, 0, 0, LV_DIR_RIGHT);
    ui->screen_home_tileview_app_tile2 = lv_tileview_add_tile(ui->screen_home_tileview_app, 1, 0, LV_DIR_LEFT);
    lv_obj_set_pos(ui->screen_home_tileview_app, 0, 0);
    lv_obj_set_size(ui->screen_home_tileview_app, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_home_tileview_app, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_home_tileview_app, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_tileview_app, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_tileview_app, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_tileview_app, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_tileview_app, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_tileview_app, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_tileview_app, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_tileview_app, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_tileview_app, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    //Write codes screen_home_btn_video
    ui->screen_home_btn_video = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_video_label = lv_label_create(ui->screen_home_btn_video);
    lv_label_set_text(ui->screen_home_btn_video_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_video_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_video_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_video, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_video_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_video, 350, 205);
    lv_obj_set_size(ui->screen_home_btn_video, 100, 100);

    //Write style for screen_home_btn_video, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_video, &_videoplayer_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_video, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_video, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_video, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_music
    ui->screen_home_btn_music = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_music_label = lv_label_create(ui->screen_home_btn_music);
    lv_label_set_text(ui->screen_home_btn_music_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_music_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_music_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_music, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_music_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_music, 200, 205);
    lv_obj_set_size(ui->screen_home_btn_music, 100, 100);

    //Write style for screen_home_btn_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_music, &_music_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_music, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_music, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_music, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_weather
    ui->screen_home_btn_weather = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_weather_label = lv_label_create(ui->screen_home_btn_weather);
    lv_label_set_text(ui->screen_home_btn_weather_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_weather_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_weather_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_weather, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_weather_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_weather, 50, 205);
    lv_obj_set_size(ui->screen_home_btn_weather, 100, 100);

    //Write style for screen_home_btn_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_weather, &_weather_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_weather, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_weather, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_weather, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_calculator
    ui->screen_home_btn_calculator = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_calculator_label = lv_label_create(ui->screen_home_btn_calculator);
    lv_label_set_text(ui->screen_home_btn_calculator_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_calculator_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_calculator_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_calculator, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_calculator_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_calculator, 650, 65);
    lv_obj_set_size(ui->screen_home_btn_calculator, 100, 100);

    //Write style for screen_home_btn_calculator, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_calculator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_calculator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_calculator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_calculator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_calculator, &_calculator2_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_calculator, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_calculator, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_calculator, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_calculator, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_calculator, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_calculator, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_setting
    ui->screen_home_btn_setting = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_setting_label = lv_label_create(ui->screen_home_btn_setting);
    lv_label_set_text(ui->screen_home_btn_setting_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_setting_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_setting_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_setting, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_setting_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_setting, 500, 65);
    lv_obj_set_size(ui->screen_home_btn_setting, 100, 100);

    //Write style for screen_home_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_setting, &_settings_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_setting, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_setting, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_pictrue
    ui->screen_home_btn_pictrue = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_pictrue_label = lv_label_create(ui->screen_home_btn_pictrue);
    lv_label_set_text(ui->screen_home_btn_pictrue_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_pictrue_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_pictrue_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_pictrue, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_pictrue_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_pictrue, 350, 65);
    lv_obj_set_size(ui->screen_home_btn_pictrue, 100, 100);

    //Write style for screen_home_btn_pictrue, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_pictrue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_pictrue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_pictrue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_pictrue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_pictrue, &_gallery3d_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_pictrue, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_pictrue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_pictrue, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_pictrue, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_pictrue, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_pictrue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_calendar
    ui->screen_home_btn_calendar = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_calendar_label = lv_label_create(ui->screen_home_btn_calendar);
    lv_label_set_text(ui->screen_home_btn_calendar_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_calendar_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_calendar_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_calendar, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_calendar_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_calendar, 200, 65);
    lv_obj_set_size(ui->screen_home_btn_calendar, 100, 100);

    //Write style for screen_home_btn_calendar, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_calendar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_calendar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_calendar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_calendar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_calendar, &_calendar_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_calendar, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_calendar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_calendar, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_calendar, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_calendar, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_calendar, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_clock
    ui->screen_home_btn_clock = lv_btn_create(ui->screen_home_tileview_app_tile1);
    ui->screen_home_btn_clock_label = lv_label_create(ui->screen_home_btn_clock);
    lv_label_set_text(ui->screen_home_btn_clock_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_clock_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_clock_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_clock, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_clock_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_clock, 50, 65);
    lv_obj_set_size(ui->screen_home_btn_clock, 100, 100);

    //Write style for screen_home_btn_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_clock, &_deskclock_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_clock, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_clock, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);



    //Write codes screen_home_btn_notes
    ui->screen_home_btn_notes = lv_btn_create(ui->screen_home_tileview_app_tile2);
    ui->screen_home_btn_notes_label = lv_label_create(ui->screen_home_btn_notes);
    lv_label_set_text(ui->screen_home_btn_notes_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_notes_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_notes_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_notes, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_notes_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_notes, 350, 65);
    lv_obj_set_size(ui->screen_home_btn_notes, 100, 100);

    //Write style for screen_home_btn_notes, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_notes, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_notes, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_notes, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_notes, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_notes, &_notes_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_notes, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_notes, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_notes, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_notes, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_notes, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_notes, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_recoder
    ui->screen_home_btn_recoder = lv_btn_create(ui->screen_home_tileview_app_tile2);
    ui->screen_home_btn_recoder_label = lv_label_create(ui->screen_home_btn_recoder);
    lv_label_set_text(ui->screen_home_btn_recoder_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_recoder_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_recoder_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_recoder, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_recoder_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_recoder, 200, 65);
    lv_obj_set_size(ui->screen_home_btn_recoder, 100, 100);

    //Write style for screen_home_btn_recoder, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_recoder, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_recoder, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_recoder, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_recoder, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_recoder, &_soundrecorder_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_recoder, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_recoder, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_recoder, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_recoder, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_recoder, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_recoder, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_ai
    ui->screen_home_btn_ai = lv_btn_create(ui->screen_home_tileview_app_tile2);
    ui->screen_home_btn_ai_label = lv_label_create(ui->screen_home_btn_ai);
    lv_label_set_text(ui->screen_home_btn_ai_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_ai_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_ai_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_ai, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_ai_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_ai, 50, 65);
    lv_obj_set_size(ui->screen_home_btn_ai, 100, 100);

    //Write style for screen_home_btn_ai, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_ai, &_voicedialer_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_ai, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_ai, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_ai, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_cont_docker
    ui->screen_home_cont_docker = lv_obj_create(ui->screen_home_cont_home);
    lv_obj_set_pos(ui->screen_home_cont_docker, 175, 340);
    lv_obj_set_size(ui->screen_home_cont_docker, 450, 110);
    lv_obj_set_scrollbar_mode(ui->screen_home_cont_docker, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_home_cont_docker, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_home_cont_docker, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_cont_docker, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_home_cont_docker, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_home_cont_docker, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_home_cont_docker, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_cont_docker, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_cont_docker, 104, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_cont_docker, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_cont_docker, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_cont_docker, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_cont_docker, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_cont_docker, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_cont_docker, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_cont_docker, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_btn_cam
    ui->screen_home_btn_cam = lv_btn_create(ui->screen_home_cont_docker);
    ui->screen_home_btn_cam_label = lv_label_create(ui->screen_home_btn_cam);
    lv_label_set_text(ui->screen_home_btn_cam_label, "");
    lv_label_set_long_mode(ui->screen_home_btn_cam_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_home_btn_cam_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_home_btn_cam, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_home_btn_cam_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_home_btn_cam, 5, 1);
    lv_obj_set_size(ui->screen_home_btn_cam, 100, 100);

    //Write style for screen_home_btn_cam, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_btn_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_btn_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_btn_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_btn_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_home_btn_cam, &_camera_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_home_btn_cam, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_home_btn_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_btn_cam, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_btn_cam, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_btn_cam, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_btn_cam, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_cont_status_bar
    ui->screen_home_cont_status_bar = lv_obj_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_cont_status_bar, 0, 0);
    lv_obj_set_size(ui->screen_home_cont_status_bar, 800, 35);
    lv_obj_set_scrollbar_mode(ui->screen_home_cont_status_bar, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_home_cont_status_bar, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_home_cont_status_bar, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_cont_status_bar, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_cont_status_bar, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_cont_status_bar, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_cont_status_bar, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_status_bt
    ui->screen_home_label_status_bt = lv_label_create(ui->screen_home_cont_status_bar);
    lv_label_set_text_static(ui->screen_home_label_status_bt, "" LV_SYMBOL_BLUETOOTH " ");
    lv_label_set_long_mode(ui->screen_home_label_status_bt, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_home_label_status_bt, 680, 8);
    lv_obj_set_size(ui->screen_home_label_status_bt, 30, 30);

    //Write style for screen_home_label_status_bt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_status_bt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_status_bt, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_status_bt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_status_bt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_status_bt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_status_eq
    ui->screen_home_label_status_eq = lv_label_create(ui->screen_home_cont_status_bar);
    lv_label_set_text_static(ui->screen_home_label_status_eq, "" LV_SYMBOL_BATTERY_FULL " ");
    lv_label_set_long_mode(ui->screen_home_label_status_eq, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_home_label_status_eq, 760, 8);
    lv_obj_set_size(ui->screen_home_label_status_eq, 30, 30);

    //Write style for screen_home_label_status_eq, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_status_eq, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_status_eq, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_status_eq, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_status_eq, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_status_eq, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_status_wifi
    ui->screen_home_label_status_wifi = lv_label_create(ui->screen_home_cont_status_bar);
    lv_label_set_text_static(ui->screen_home_label_status_wifi, "" LV_SYMBOL_WIFI " ");
    lv_label_set_long_mode(ui->screen_home_label_status_wifi, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_home_label_status_wifi, 718, 7);
    lv_obj_set_size(ui->screen_home_label_status_wifi, 30, 30);

    //Write style for screen_home_label_status_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_status_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_status_wifi, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_status_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_status_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_status_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_digital_clock_status
    static bool screen_home_digital_clock_status_timer_enabled = false;
    // // 获取当前系统时间
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char time_str[20];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
                timeinfo->tm_hour, 
                timeinfo->tm_min, 
                timeinfo->tm_sec);
    // 使用当前时间作为初始值
    ui->screen_home_digital_clock_status = lv_dclock_create(ui->screen_home_cont_status_bar, time_str);
    // 获取当前系统时间
    if (!screen_home_digital_clock_status_timer_enabled) {
        // lv_timer_create(screen_home_digital_clock_status_timer, 1000, NULL);
        screen_home_digital_clock_status_timer_enabled = true;
    }

    //Write style for screen_home_digital_clock_status, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_digital_clock_status, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_digital_clock_status, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_digital_clock_status, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_digital_clock_status, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_digital_clock_status, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_digital_clock_status, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_digital_clock_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_home.
    // lv_obj_clear_flag(ui->screen_home, LV_OBJ_FLAG_CLICKABLE);
// lv_obj_clear_flag(ui->screen_home_cont_home, LV_OBJ_FLAG_CLICKABLE);
// lv_obj_clear_flag(ui->screen_home_cont_status_bar, LV_OBJ_FLAG_CLICKABLE);

// lv_obj_set_tile(guider_ui.screen_home_tileview_select, guider_ui.screen_home_tileview_select_tile_center, LV_ANIM_OFF);
// lv_obj_clear_flag(guider_ui.screen_home_cont_top, LV_OBJ_FLAG_PRESS_LOCK); //Allow press event to tileview
// lv_obj_clear_flag(guider_ui.screen_home_cont_bottom, LV_OBJ_FLAG_PRESS_LOCK); //Allow press event to tileview
    lv_obj_set_user_data(ui->screen_home_slider_light, ui->screen_home_label_light);
    lv_obj_set_user_data(ui->screen_home_slider_volume, ui->screen_home_label_volume);

    custom_top_init(ui);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_home);

    //Init events for screen.
    events_init_screen_home(ui);
}