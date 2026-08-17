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



void setup_scr_screen_video(lv_ui *ui)
{
    //Write codes screen_video
    ui->screen_video = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_video, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_video, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_video, LV_OBJ_FLAG_CLICKABLE);

    //Write style for screen_video, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_cont_video
    ui->screen_video_cont_video = lv_obj_create(ui->screen_video);
    lv_obj_set_pos(ui->screen_video_cont_video, 0, 0);
    lv_obj_set_size(ui->screen_video_cont_video, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_video_cont_video, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_video_cont_video, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_cont_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_cont_video, lv_color_hex(0x070707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_cont_video, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_cont_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_img_video
    ui->screen_video_img_video = lv_img_create(ui->screen_video_cont_video);
    lv_obj_add_flag(ui->screen_video_img_video, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_video_img_video, &_noVideo_alpha_800x450);
    lv_img_set_pivot(ui->screen_video_img_video, 50,50);
    lv_img_set_angle(ui->screen_video_img_video, 0);
    lv_obj_set_pos(ui->screen_video_img_video, 0, 15);
    lv_obj_set_size(ui->screen_video_img_video, 800, 450);

    //Write style for screen_video_img_video, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_video_img_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_video_img_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_img_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_video_img_video, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_img_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_img_video, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_cont_videoCtrl
    ui->screen_video_cont_videoCtrl = lv_obj_create(ui->screen_video);
    lv_obj_set_pos(ui->screen_video_cont_videoCtrl, 0, 0);
    lv_obj_set_size(ui->screen_video_cont_videoCtrl, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_video_cont_videoCtrl, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_video_cont_videoCtrl, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_video_cont_videoCtrl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_cont_videoCtrl, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_bar_time
    ui->screen_video_bar_time = lv_bar_create(ui->screen_video_cont_videoCtrl);
    lv_obj_set_style_anim_time(ui->screen_video_bar_time, 1000, 0);
    lv_bar_set_mode(ui->screen_video_bar_time, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_video_bar_time, 0, 100);
    lv_bar_set_value(ui->screen_video_bar_time, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_video_bar_time, 25, 400);
    lv_obj_set_size(ui->screen_video_bar_time, 750, 6);

    //Write style for screen_video_bar_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_bar_time, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_bar_time, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_bar_time, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_bar_time, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_bar_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_video_bar_time, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_bar_time, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_bar_time, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_bar_time, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_bar_time, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_playing
    ui->screen_video_btn_playing = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_playing_label = lv_label_create(ui->screen_video_btn_playing);
    lv_label_set_text(ui->screen_video_btn_playing_label, " " LV_SYMBOL_PLAY " ");
    lv_label_set_long_mode(ui->screen_video_btn_playing_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_playing_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_playing, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_playing_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_playing, 129, 420);
    lv_obj_set_size(ui->screen_video_btn_playing, 100, 50);

    //Write style for screen_video_btn_playing, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_playing, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_playing, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_playing, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_playing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_playing, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_playing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_playing, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_playing, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_playing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_playing, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_prev
    ui->screen_video_btn_prev = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_prev_label = lv_label_create(ui->screen_video_btn_prev);
    lv_label_set_text(ui->screen_video_btn_prev_label, " " LV_SYMBOL_PREV " ");
    lv_label_set_long_mode(ui->screen_video_btn_prev_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_prev_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_prev, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_prev_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_prev, 9, 420);
    lv_obj_set_size(ui->screen_video_btn_prev, 100, 50);

    //Write style for screen_video_btn_prev, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_prev, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_prev, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_prev, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_prev, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_prev, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_prev, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_prev, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_prev, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_next
    ui->screen_video_btn_next = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_next_label = lv_label_create(ui->screen_video_btn_next);
    lv_label_set_text(ui->screen_video_btn_next_label, "" LV_SYMBOL_NEXT "");
    lv_label_set_long_mode(ui->screen_video_btn_next_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_next_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_next, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_next_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_next, 249, 420);
    lv_obj_set_size(ui->screen_video_btn_next, 100, 50);

    //Write style for screen_video_btn_next, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_next, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_next, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_next, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_next, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_next, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_next, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_next, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_next, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_close
    ui->screen_video_btn_close = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_close_label = lv_label_create(ui->screen_video_btn_close);
    lv_label_set_text(ui->screen_video_btn_close_label, " " LV_SYMBOL_STOP " ");
    lv_label_set_long_mode(ui->screen_video_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_close_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_close, 368, 420);
    lv_obj_set_size(ui->screen_video_btn_close, 100, 50);

    //Write style for screen_video_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_close, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_close, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_close, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_close, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_close, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_list
    ui->screen_video_btn_list = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_list_label = lv_label_create(ui->screen_video_btn_list);
    lv_label_set_text(ui->screen_video_btn_list_label, " " LV_SYMBOL_LIST " ");
    lv_label_set_long_mode(ui->screen_video_btn_list_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_list_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_list, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_list_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_list, 486, 420);
    lv_obj_set_size(ui->screen_video_btn_list, 100, 50);

    //Write style for screen_video_btn_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_list, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_list, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_list, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_list, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_list, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_list, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_list, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_label_fileName
    ui->screen_video_label_fileName = lv_label_create(ui->screen_video_cont_videoCtrl);
    lv_label_set_text(ui->screen_video_label_fileName, "text.mp4");
    lv_label_set_long_mode(ui->screen_video_label_fileName, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_video_label_fileName, 1, 0);
    lv_obj_set_size(ui->screen_video_label_fileName, 800, 32);

    //Write style for screen_video_label_fileName, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_label_fileName, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_label_fileName, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_label_fileName, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_label_fileName, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_label_fileName, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_label_fileName, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_label_fileName, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_label_progressTime
    ui->screen_video_label_progressTime = lv_label_create(ui->screen_video_cont_videoCtrl);
    lv_label_set_text(ui->screen_video_label_progressTime, "00：00/30：00");
    lv_label_set_long_mode(ui->screen_video_label_progressTime, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_video_label_progressTime, 590, 430);
    lv_obj_set_size(ui->screen_video_label_progressTime, 210, 30);

    //Write style for screen_video_label_progressTime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_label_progressTime, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_label_progressTime, &lv_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_label_progressTime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_label_progressTime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_label_progressTime, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_label_progressTime, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_label_progressTime, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_btn_back
    ui->screen_video_btn_back = lv_btn_create(ui->screen_video_cont_videoCtrl);
    ui->screen_video_btn_back_label = lv_label_create(ui->screen_video_btn_back);
    lv_label_set_text(ui->screen_video_btn_back_label, " " LV_SYMBOL_LEFT " ");
    lv_label_set_long_mode(ui->screen_video_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_back_label, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_back, 0, 0);
    lv_obj_set_size(ui->screen_video_btn_back, 50, 32);

    //Write style for screen_video_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_back, &lv_font_SourceHanSerifSC_Regular_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_back, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_cont_list
    ui->screen_video_cont_list = lv_obj_create(ui->screen_video);
    lv_obj_set_pos(ui->screen_video_cont_list, 580, 0);
    lv_obj_set_size(ui->screen_video_cont_list, 220, 480);
    lv_obj_set_scrollbar_mode(ui->screen_video_cont_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->screen_video_cont_list, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_video_cont_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_video_list_video
    ui->screen_video_list_video = lv_list_create(ui->screen_video_cont_list);
    ui->screen_video_list_video_item0 = lv_list_add_btn(ui->screen_video_list_video, LV_SYMBOL_VIDEO, "test.mp4");
    ui->screen_video_list_video_item1 = lv_list_add_btn(ui->screen_video_list_video, LV_SYMBOL_VIDEO, "test.avi");
    lv_obj_set_pos(ui->screen_video_list_video, 20, 0);
    lv_obj_set_size(ui->screen_video_list_video, 200, 480);
    lv_obj_set_scrollbar_mode(ui->screen_video_list_video, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_screen_video_list_video_main_main_default
    static lv_style_t style_screen_video_list_video_main_main_default;
    ui_init_style(&style_screen_video_list_video_main_main_default);

    lv_style_set_pad_top(&style_screen_video_list_video_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_video_list_video_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_video_list_video_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_video_list_video_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_video_list_video_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_video_list_video_main_main_default, lv_color_hex(0x242424));
    lv_style_set_bg_grad_dir(&style_screen_video_list_video_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_video_list_video_main_main_default, 0);
    lv_style_set_radius(&style_screen_video_list_video_main_main_default, 0);
    lv_style_set_shadow_width(&style_screen_video_list_video_main_main_default, 0);
    lv_obj_add_style(ui->screen_video_list_video, &style_screen_video_list_video_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_video_list_video_main_scrollbar_default
    static lv_style_t style_screen_video_list_video_main_scrollbar_default;
    ui_init_style(&style_screen_video_list_video_main_scrollbar_default);

    lv_style_set_radius(&style_screen_video_list_video_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_video_list_video_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_video_list_video_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_video_list_video_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_video_list_video, &style_screen_video_list_video_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_video_list_video_extra_btns_main_default
    static lv_style_t style_screen_video_list_video_extra_btns_main_default;
    ui_init_style(&style_screen_video_list_video_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_video_list_video_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_video_list_video_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_video_list_video_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_video_list_video_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_video_list_video_extra_btns_main_default, 2);
    lv_style_set_border_opa(&style_screen_video_list_video_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_screen_video_list_video_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_video_list_video_extra_btns_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_screen_video_list_video_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_video_list_video_extra_btns_main_default, &lv_font_SourceHanSerifSC_Regular_18);
    lv_style_set_text_opa(&style_screen_video_list_video_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_video_list_video_extra_btns_main_default, 5);
    lv_style_set_bg_opa(&style_screen_video_list_video_extra_btns_main_default, 91);
    lv_style_set_bg_color(&style_screen_video_list_video_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_video_list_video_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_video_list_video_item1, &style_screen_video_list_video_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_video_list_video_item0, &style_screen_video_list_video_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_video_list_video_extra_texts_main_default
    static lv_style_t style_screen_video_list_video_extra_texts_main_default;
    ui_init_style(&style_screen_video_list_video_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_video_list_video_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_video_list_video_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_video_list_video_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_video_list_video_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_video_list_video_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_video_list_video_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_video_list_video_extra_texts_main_default, &lv_font_SourceHanSerifSC_Regular_12);
    lv_style_set_text_opa(&style_screen_video_list_video_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_video_list_video_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_video_list_video_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_video_list_video_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_video_list_video_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_video_list_video_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_video_btn_closeList
    ui->screen_video_btn_closeList = lv_btn_create(ui->screen_video_cont_list);
    ui->screen_video_btn_closeList_label = lv_label_create(ui->screen_video_btn_closeList);
    lv_label_set_text(ui->screen_video_btn_closeList_label, " " LV_SYMBOL_RIGHT " ");
    lv_label_set_long_mode(ui->screen_video_btn_closeList_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_video_btn_closeList_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_video_btn_closeList, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_video_btn_closeList_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_video_btn_closeList, 0, 0);
    lv_obj_set_size(ui->screen_video_btn_closeList, 20, 480);

    //Write style for screen_video_btn_closeList, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_video_btn_closeList, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_video_btn_closeList, lv_color_hex(0x1a1a1a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_video_btn_closeList, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_video_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_video_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_video_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_video_btn_closeList, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_video_btn_closeList, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_video_btn_closeList, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_video_btn_closeList, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_video.
    lv_obj_clear_flag(ui->screen_video, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_video_cont_video, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_video_cont_videoCtrl, LV_OBJ_FLAG_SCROLLABLE);



    //Update current screen layout.
    lv_obj_update_layout(ui->screen_video);

    //Init events for screen.
    events_init_screen_video(ui);
}