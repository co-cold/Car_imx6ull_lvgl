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



void setup_scr_screen_music(lv_ui *ui)
{
    //Write codes screen_music
    ui->screen_music = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_music, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_music, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_cont_music
    ui->screen_music_cont_music = lv_obj_create(ui->screen_music);
    lv_obj_set_pos(ui->screen_music_cont_music, 0, 0);
    lv_obj_set_size(ui->screen_music_cont_music, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_music_cont_music, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_music_cont_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_cont_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_cont_music, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_cont_music, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_music_cont_music, &_music_bck_800x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_music_cont_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_cont_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_img_music
    ui->screen_music_img_music = lv_img_create(ui->screen_music_cont_music);
    lv_obj_add_flag(ui->screen_music_img_music, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_music_img_music, &_1725420839434_alpha_250x250);
    lv_img_set_pivot(ui->screen_music_img_music, 50,50);
    lv_img_set_angle(ui->screen_music_img_music, 0);
    lv_obj_set_pos(ui->screen_music_img_music, 30, 30);
    lv_obj_set_size(ui->screen_music_img_music, 250, 250);

    //Write style for screen_music_img_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_music_img_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_music_img_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_img_music, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_music_img_music, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_btn_next
    ui->screen_music_btn_next = lv_btn_create(ui->screen_music_cont_music);
    ui->screen_music_btn_next_label = lv_label_create(ui->screen_music_btn_next);
    lv_label_set_text(ui->screen_music_btn_next_label, "下一首");
    lv_label_set_long_mode(ui->screen_music_btn_next_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_music_btn_next_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_music_btn_next, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_music_btn_next_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_music_btn_next, 360, 360);
    lv_obj_set_size(ui->screen_music_btn_next, 100, 50);

    //Write style for screen_music_btn_next, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_btn_next, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_btn_next, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_btn_next, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_music_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_btn_next, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_btn_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_btn_next, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_btn_next, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_btn_next, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_btn_next, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_btn_prev
    ui->screen_music_btn_prev = lv_btn_create(ui->screen_music_cont_music);
    ui->screen_music_btn_prev_label = lv_label_create(ui->screen_music_btn_prev);
    lv_label_set_text(ui->screen_music_btn_prev_label, "上一首");
    lv_label_set_long_mode(ui->screen_music_btn_prev_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_music_btn_prev_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_music_btn_prev, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_music_btn_prev_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_music_btn_prev, 50, 360);
    lv_obj_set_size(ui->screen_music_btn_prev, 100, 50);

    //Write style for screen_music_btn_prev, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_btn_prev, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_btn_prev, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_btn_prev, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_music_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_btn_prev, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_btn_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_btn_prev, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_btn_prev, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_btn_prev, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_btn_prev, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_btn_playing
    ui->screen_music_btn_playing = lv_btn_create(ui->screen_music_cont_music);
    ui->screen_music_btn_playing_label = lv_label_create(ui->screen_music_btn_playing);
    lv_label_set_text(ui->screen_music_btn_playing_label, "播放");
    lv_label_set_long_mode(ui->screen_music_btn_playing_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_music_btn_playing_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_music_btn_playing, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_music_btn_playing_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_music_btn_playing, 205, 360);
    lv_obj_set_size(ui->screen_music_btn_playing, 100, 50);

    //Write style for screen_music_btn_playing, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_btn_playing, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_btn_playing, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_btn_playing, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_music_btn_playing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_btn_playing, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_btn_playing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_btn_playing, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_btn_playing, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_btn_playing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_btn_playing, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_btn_list
    ui->screen_music_btn_list = lv_btn_create(ui->screen_music_cont_music);
    ui->screen_music_btn_list_label = lv_label_create(ui->screen_music_btn_list);
    lv_label_set_text(ui->screen_music_btn_list_label, " " LV_SYMBOL_LIST " ");
    lv_label_set_long_mode(ui->screen_music_btn_list_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_music_btn_list_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_music_btn_list, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_music_btn_list_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_music_btn_list, 670, 360);
    lv_obj_set_size(ui->screen_music_btn_list, 100, 50);

    //Write style for screen_music_btn_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_btn_list, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_btn_list, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_btn_list, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_music_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_btn_list, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_btn_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_btn_list, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_btn_list, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_btn_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_btn_list, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_line_1
    ui->screen_music_line_1 = lv_line_create(ui->screen_music_cont_music);
    static lv_point_t screen_music_line_1[] = {{10, 0},{790, 0},};
    lv_line_set_points(ui->screen_music_line_1, screen_music_line_1, 2);
    lv_obj_set_pos(ui->screen_music_line_1, 0, 330);
    lv_obj_set_size(ui->screen_music_line_1, 800, 5);

    //Write style for screen_music_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_music_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_music_line_1, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_music_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_music_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_slider_progressTime
    ui->screen_music_slider_progressTime = lv_slider_create(ui->screen_music_cont_music);
    lv_slider_set_range(ui->screen_music_slider_progressTime, 0, 100);
    lv_slider_set_mode(ui->screen_music_slider_progressTime, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_music_slider_progressTime, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_music_slider_progressTime, 380, 286);
    lv_obj_set_size(ui->screen_music_slider_progressTime, 350, 12);

    //Write style for screen_music_slider_progressTime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_slider_progressTime, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_slider_progressTime, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_slider_progressTime, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->screen_music_slider_progressTime, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->screen_music_slider_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->screen_music_slider_progressTime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_slider_progressTime, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_music_slider_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_slider_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_music_slider_progressTime, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_slider_progressTime, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_slider_progressTime, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_slider_progressTime, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_slider_progressTime, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_music_slider_progressTime, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_slider_progressTime, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_slider_progressTime, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_music_label_lyric
    ui->screen_music_label_lyric = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text_static(ui->screen_music_label_lyric, "歌词");
    lv_label_set_long_mode(ui->screen_music_label_lyric, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_music_label_lyric, 399, 25);
    lv_obj_set_size(ui->screen_music_label_lyric, 300, 200);

    //Write style for screen_music_label_lyric, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_lyric, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_lyric, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_lyric, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_lyric, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_lyric, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_label_lyric, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_label_lyric, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_lyric, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_label_progressTime
    ui->screen_music_label_progressTime = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text(ui->screen_music_label_progressTime, "Label");
    lv_label_set_long_mode(ui->screen_music_label_progressTime, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_music_label_progressTime, 285, 286);
    lv_obj_set_size(ui->screen_music_label_progressTime, 100, 32);

    //Write style for screen_music_label_progressTime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_progressTime, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_progressTime, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_progressTime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_progressTime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_progressTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_label_endTime
    ui->screen_music_label_endTime = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text(ui->screen_music_label_endTime, "Label");
    lv_label_set_long_mode(ui->screen_music_label_endTime, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_music_label_endTime, 717, 286);
    lv_obj_set_size(ui->screen_music_label_endTime, 100, 32);

    //Write style for screen_music_label_endTime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_endTime, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_endTime, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_endTime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_endTime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_endTime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_label_singer
    ui->screen_music_label_singer = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text(ui->screen_music_label_singer, "歌手");
    lv_label_set_long_mode(ui->screen_music_label_singer, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_music_label_singer, 580, 243);
    lv_obj_set_size(ui->screen_music_label_singer, 200, 32);

    //Write style for screen_music_label_singer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_singer, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_singer, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_singer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_singer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_singer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_label_song
    ui->screen_music_label_song = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text(ui->screen_music_label_song, "名称");
    lv_label_set_long_mode(ui->screen_music_label_song, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_music_label_song, 350, 243);
    lv_obj_set_size(ui->screen_music_label_song, 200, 32);

    //Write style for screen_music_label_song, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_song, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_song, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_song, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_song, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_song, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_label_fileName
    ui->screen_music_label_fileName = lv_label_create(ui->screen_music_cont_music);
    lv_label_set_text(ui->screen_music_label_fileName, "filename");
    lv_label_set_long_mode(ui->screen_music_label_fileName, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_music_label_fileName, 5, 285);
    lv_obj_set_size(ui->screen_music_label_fileName, 300, 40);

    //Write style for screen_music_label_fileName, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_label_fileName, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_label_fileName, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_label_fileName, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_label_fileName, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_label_fileName, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_cont_list
    ui->screen_music_cont_list = lv_obj_create(ui->screen_music);
    lv_obj_set_pos(ui->screen_music_cont_list, 0, 480);
    lv_obj_set_size(ui->screen_music_cont_list, 800, 300);
    lv_obj_set_scrollbar_mode(ui->screen_music_cont_list, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_music_cont_list, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_music_cont_list, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_music_cont_list, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_music_cont_list, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_cont_list, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_img_listBck
    ui->screen_music_img_listBck = lv_img_create(ui->screen_music_cont_list);
    lv_obj_add_flag(ui->screen_music_img_listBck, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_music_img_listBck, &_music_bck_alpha_800x300);
    lv_img_set_pivot(ui->screen_music_img_listBck, 50,50);
    lv_img_set_angle(ui->screen_music_img_listBck, 0);
    lv_obj_set_pos(ui->screen_music_img_listBck, 0, 0);
    lv_obj_set_size(ui->screen_music_img_listBck, 800, 300);

    //Write style for screen_music_img_listBck, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_music_img_listBck, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_music_img_listBck, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_img_listBck, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_music_img_listBck, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_music_list_music
    ui->screen_music_list_music = lv_list_create(ui->screen_music_cont_list);
    ui->screen_music_list_music_item0 = lv_list_add_btn(ui->screen_music_list_music, LV_SYMBOL_AUDIO, "歌曲1");
    ui->screen_music_list_music_item1 = lv_list_add_btn(ui->screen_music_list_music, LV_SYMBOL_AUDIO, "歌曲2");
    ui->screen_music_list_music_item2 = lv_list_add_btn(ui->screen_music_list_music, LV_SYMBOL_AUDIO, "歌曲3");
    lv_obj_set_pos(ui->screen_music_list_music, 0, 25);
    lv_obj_set_size(ui->screen_music_list_music, 800, 275);
    lv_obj_set_scrollbar_mode(ui->screen_music_list_music, LV_SCROLLBAR_MODE_ON);

    //Write style state: LV_STATE_DEFAULT for &style_screen_music_list_music_main_main_default
    static lv_style_t style_screen_music_list_music_main_main_default;
    ui_init_style(&style_screen_music_list_music_main_main_default);

    lv_style_set_pad_top(&style_screen_music_list_music_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_pad_right(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_pad_bottom(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_bg_opa(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_border_width(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_radius(&style_screen_music_list_music_main_main_default, 0);
    lv_style_set_shadow_width(&style_screen_music_list_music_main_main_default, 0);
    lv_obj_add_style(ui->screen_music_list_music, &style_screen_music_list_music_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_music_list_music_main_scrollbar_default
    static lv_style_t style_screen_music_list_music_main_scrollbar_default;
    ui_init_style(&style_screen_music_list_music_main_scrollbar_default);

    lv_style_set_radius(&style_screen_music_list_music_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_music_list_music_main_scrollbar_default, 0);
    lv_obj_add_style(ui->screen_music_list_music, &style_screen_music_list_music_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_music_list_music_extra_btns_main_default
    static lv_style_t style_screen_music_list_music_extra_btns_main_default;
    ui_init_style(&style_screen_music_list_music_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_music_list_music_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_music_list_music_extra_btns_main_default, 10);
    lv_style_set_pad_right(&style_screen_music_list_music_extra_btns_main_default, 0);
    lv_style_set_pad_bottom(&style_screen_music_list_music_extra_btns_main_default, 10);
    lv_style_set_border_width(&style_screen_music_list_music_extra_btns_main_default, 1);
    lv_style_set_border_opa(&style_screen_music_list_music_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_screen_music_list_music_extra_btns_main_default, lv_color_hex(0x6f6969));
    lv_style_set_border_side(&style_screen_music_list_music_extra_btns_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_screen_music_list_music_extra_btns_main_default, lv_color_hex(0xfef6ea));
    lv_style_set_text_font(&style_screen_music_list_music_extra_btns_main_default, &lv_font_SourceHanSerifSC_Regular_18);
    lv_style_set_text_opa(&style_screen_music_list_music_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_music_list_music_extra_btns_main_default, 0);
    lv_style_set_bg_opa(&style_screen_music_list_music_extra_btns_main_default, 0);
    lv_obj_add_style(ui->screen_music_list_music_item2, &style_screen_music_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_music_list_music_item1, &style_screen_music_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_music_list_music_item0, &style_screen_music_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_music_list_music_extra_texts_main_default
    static lv_style_t style_screen_music_list_music_extra_texts_main_default;
    ui_init_style(&style_screen_music_list_music_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_music_list_music_extra_texts_main_default, 6);
    lv_style_set_pad_left(&style_screen_music_list_music_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_music_list_music_extra_texts_main_default, 0);
    lv_style_set_pad_bottom(&style_screen_music_list_music_extra_texts_main_default, 0);
    lv_style_set_border_width(&style_screen_music_list_music_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_music_list_music_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_music_list_music_extra_texts_main_default, &lv_font_montserratMedium_18);
    lv_style_set_text_opa(&style_screen_music_list_music_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_music_list_music_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_music_list_music_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_music_list_music_extra_texts_main_default, 0);

    //Write codes screen_music_btn_closeList
    ui->screen_music_btn_closeList = lv_btn_create(ui->screen_music_cont_list);
    ui->screen_music_btn_closeList_label = lv_label_create(ui->screen_music_btn_closeList);
    lv_label_set_text(ui->screen_music_btn_closeList_label, " " LV_SYMBOL_DOWN " ");
    lv_label_set_long_mode(ui->screen_music_btn_closeList_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_music_btn_closeList_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_music_btn_closeList, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_music_btn_closeList_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_music_btn_closeList, 0, 0);
    lv_obj_set_size(ui->screen_music_btn_closeList, 800, 25);

    //Write style for screen_music_btn_closeList, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_music_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_music_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_music_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_music_btn_closeList, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_music_btn_closeList, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_music_btn_closeList, &lv_font_SourceHanSerifSC_Regular_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_music_btn_closeList, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_music_btn_closeList, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_music.
    lv_obj_clear_flag(ui->screen_music, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_music_cont_music, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_music_cont_list, LV_OBJ_FLAG_SCROLLABLE);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_music);

    //Init events for screen.
    events_init_screen_music(ui);
}
