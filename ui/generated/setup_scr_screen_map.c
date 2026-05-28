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



void setup_scr_screen_map(lv_ui *ui)
{
    //Write codes screen_map
    ui->screen_map = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_map, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_map, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_map, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_map, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_map_imgbtn_map
    ui->screen_map_imgbtn_map = lv_imgbtn_create(ui->screen_map);
    lv_obj_add_flag(ui->screen_map_imgbtn_map, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->screen_map_imgbtn_map, LV_IMGBTN_STATE_RELEASED, NULL, &_weatherBK_alpha_800x480, NULL);
    ui->screen_map_imgbtn_map_label = lv_label_create(ui->screen_map_imgbtn_map);
    lv_label_set_text(ui->screen_map_imgbtn_map_label, "");
    lv_label_set_long_mode(ui->screen_map_imgbtn_map_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_map_imgbtn_map_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_map_imgbtn_map, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_map_imgbtn_map, 0, 0);
    lv_obj_set_size(ui->screen_map_imgbtn_map, 800, 480);

    //Write style for screen_map_imgbtn_map, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_map_imgbtn_map, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_map_imgbtn_map, &lv_font_montserratMedium_58, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_map_imgbtn_map, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_map_imgbtn_map, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_map_imgbtn_map, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_map_imgbtn_map, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_map_imgbtn_map, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_map_imgbtn_map, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_map_imgbtn_map, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_map_imgbtn_map, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_map_imgbtn_map, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->screen_map_imgbtn_map, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->screen_map_imgbtn_map, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //The custom code of screen_map.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_map);

}
