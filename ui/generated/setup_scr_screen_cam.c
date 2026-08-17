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
    lv_obj_set_style_bg_color(ui->screen_cam_cont_cam, lv_color_hex(0x070707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_cont_cam, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_cam_cont_cam, &_musicBack2l1_800x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_cam_cont_cam, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_cont_cam, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_btn_close
    ui->screen_cam_btn_close = lv_btn_create(ui->screen_cam_cont_cam);
    ui->screen_cam_btn_close_label = lv_label_create(ui->screen_cam_btn_close);
    lv_label_set_text(ui->screen_cam_btn_close_label, "返回");
    lv_label_set_long_mode(ui->screen_cam_btn_close_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_cam_btn_close_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_cam_btn_close, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_cam_btn_close_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_cam_btn_close, 690, 415);
    lv_obj_set_size(ui->screen_cam_btn_close, 100, 50);

    //Write style for screen_cam_btn_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_cam_btn_close, 91, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_btn_close, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_btn_close, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_cam_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_btn_close, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_btn_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_cam_btn_close, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cam_btn_close, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cam_btn_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_cam_btn_close, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

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
    lv_obj_set_style_bg_opa(ui->screen_cam_img_video, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_img_video, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_cam_ddlist_device
    ui->screen_cam_ddlist_device = lv_dropdown_create(ui->screen_cam_cont_cam);
    lv_dropdown_set_options(ui->screen_cam_ddlist_device, "设备1");
    lv_obj_set_pos(ui->screen_cam_ddlist_device, 647, 15);
    lv_obj_set_size(ui->screen_cam_ddlist_device, 150, 35);

    //Write style for screen_cam_ddlist_device, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_cam_ddlist_device, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cam_ddlist_device, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cam_ddlist_device, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_cam_ddlist_device, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_cam_ddlist_device, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_cam_ddlist_device, lv_color_hex(0xe1e6ee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_cam_ddlist_device, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cam_ddlist_device, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cam_ddlist_device, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cam_ddlist_device, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cam_ddlist_device, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cam_ddlist_device, 90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cam_ddlist_device, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cam_ddlist_device, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cam_ddlist_device, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_screen_cam_ddlist_device_extra_list_selected_checked
    static lv_style_t style_screen_cam_ddlist_device_extra_list_selected_checked;
    ui_init_style(&style_screen_cam_ddlist_device_extra_list_selected_checked);

    lv_style_set_border_width(&style_screen_cam_ddlist_device_extra_list_selected_checked, 2);
    lv_style_set_border_opa(&style_screen_cam_ddlist_device_extra_list_selected_checked, 255);
    lv_style_set_border_color(&style_screen_cam_ddlist_device_extra_list_selected_checked, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_cam_ddlist_device_extra_list_selected_checked, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_cam_ddlist_device_extra_list_selected_checked, 5);
    lv_style_set_bg_opa(&style_screen_cam_ddlist_device_extra_list_selected_checked, 91);
    lv_style_set_bg_color(&style_screen_cam_ddlist_device_extra_list_selected_checked, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_cam_ddlist_device_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_cam_ddlist_device), &style_screen_cam_ddlist_device_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_cam_ddlist_device_extra_list_main_default
    static lv_style_t style_screen_cam_ddlist_device_extra_list_main_default;
    ui_init_style(&style_screen_cam_ddlist_device_extra_list_main_default);

    lv_style_set_max_height(&style_screen_cam_ddlist_device_extra_list_main_default, 90);
    lv_style_set_text_color(&style_screen_cam_ddlist_device_extra_list_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_cam_ddlist_device_extra_list_main_default, &lv_font_SourceHanSerifSC_Regular_16);
    lv_style_set_text_opa(&style_screen_cam_ddlist_device_extra_list_main_default, 255);
    lv_style_set_border_width(&style_screen_cam_ddlist_device_extra_list_main_default, 1);
    lv_style_set_border_opa(&style_screen_cam_ddlist_device_extra_list_main_default, 255);
    lv_style_set_border_color(&style_screen_cam_ddlist_device_extra_list_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_screen_cam_ddlist_device_extra_list_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_cam_ddlist_device_extra_list_main_default, 5);
    lv_style_set_bg_opa(&style_screen_cam_ddlist_device_extra_list_main_default, 91);
    lv_style_set_bg_color(&style_screen_cam_ddlist_device_extra_list_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_cam_ddlist_device_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_cam_ddlist_device), &style_screen_cam_ddlist_device_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_cam_ddlist_device_extra_list_scrollbar_default
    static lv_style_t style_screen_cam_ddlist_device_extra_list_scrollbar_default;
    ui_init_style(&style_screen_cam_ddlist_device_extra_list_scrollbar_default);

    lv_style_set_radius(&style_screen_cam_ddlist_device_extra_list_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_cam_ddlist_device_extra_list_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_cam_ddlist_device_extra_list_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_cam_ddlist_device_extra_list_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->screen_cam_ddlist_device), &style_screen_cam_ddlist_device_extra_list_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //The custom code of screen_cam.
    lv_obj_clear_flag(ui->screen_cam, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui->screen_cam_cont_cam, LV_OBJ_FLAG_CLICKABLE);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_cam);

    //Init events for screen.
    events_init_screen_cam(ui);
}