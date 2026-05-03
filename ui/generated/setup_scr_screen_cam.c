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



void setup_scr_screen_cam(lv_ui *ui)
{
    //Write codes screen_cam
    ui->screen_cam = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_cam, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_cam, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_cam, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_cont_cam
    ui->screen_cam_cont_cam = lv_obj_create(ui->screen_cam);
    lv_obj_set_pos(ui->screen_cam_cont_cam, 0, 0);
    lv_obj_set_size(ui->screen_cam_cont_cam, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_cam_cont_cam, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_cam_cont_cam, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cam_cont_cam, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_cont_cam, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_cont_cam, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_btn_close
    ui->screen_cam_btn_close = lv_btn_create(ui->screen_cam_cont_cam);
    ui->screen_cam_btn_close_label = lv_label_create(ui->screen_cam_btn_close);
    lv_label_set_text(ui->screen_cam_btn_close_label, "返回");
    lv_label_set_long_mode(ui->screen_cam_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_cam_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_cam_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_cam_btn_close_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_cam_btn_close, 690, 10);
    lv_obj_set_size(ui->screen_cam_btn_close, 100, 50);

    //Write style for screen_cam_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_cam_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_btn_close, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_btn_close, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_cam_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_btn_close, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_cam_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cam_btn_close, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cam_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_cam_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_btn_1
    ui->screen_cam_btn_1 = lv_btn_create(ui->screen_cam_cont_cam);
    ui->screen_cam_btn_1_label = lv_label_create(ui->screen_cam_btn_1);
    lv_label_set_text(ui->screen_cam_btn_1_label, "拍照");
    lv_label_set_long_mode(ui->screen_cam_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_cam_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_cam_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_cam_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_cam_btn_1, 690, 400);
    lv_obj_set_size(ui->screen_cam_btn_1, 100, 50);

    //Write style for screen_cam_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_cam_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_cam_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_cam_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cam_btn_1, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cam_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_cam_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_img_video
    ui->screen_cam_img_video = lv_img_create(ui->screen_cam_cont_cam);
    lv_obj_add_flag(ui->screen_cam_img_video, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_pivot(ui->screen_cam_img_video, 50,50);
    lv_img_set_angle(ui->screen_cam_img_video, 0);
    lv_obj_set_pos(ui->screen_cam_img_video, 0, 0);
    lv_obj_set_size(ui->screen_cam_img_video, 640, 480);

    //Write style for screen_cam_img_video, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_cam_img_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_cam_img_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_img_video, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_cam_img_video, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_cam.
    lv_obj_clear_flag(ui->screen_cam, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui->screen_cam_cont_cam, LV_OBJ_FLAG_CLICKABLE);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_cam);

    //Init events for screen.
    events_init_screen_cam(ui);
}
