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



void setup_scr_screen_weather(lv_ui *ui)
{
    //Write codes screen_weather
    ui->screen_weather = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_weather, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_weather, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_cont_weather
    ui->screen_weather_cont_weather = lv_obj_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_cont_weather, 0, 0);
    lv_obj_set_size(ui->screen_weather_cont_weather, 800, 480);
    lv_obj_set_scrollbar_mode(ui->screen_weather_cont_weather, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_weather_cont_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_cont_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_cont_weather, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_cont_weather, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_weather_cont_weather, &_weatherBK_800x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_weather_cont_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_cont_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_ta_city
    ui->screen_weather_ta_city = lv_textarea_create(ui->screen_weather_cont_weather);
    lv_textarea_set_text(ui->screen_weather_ta_city, "Hello World");
    lv_textarea_set_placeholder_text(ui->screen_weather_ta_city, "");
    lv_textarea_set_password_bullet(ui->screen_weather_ta_city, "*");
    lv_textarea_set_password_mode(ui->screen_weather_ta_city, false);
    lv_textarea_set_one_line(ui->screen_weather_ta_city, false);
    lv_textarea_set_accepted_chars(ui->screen_weather_ta_city, "");
    lv_textarea_set_max_length(ui->screen_weather_ta_city, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_weather_ta_city, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->screen_weather_ta_city, 380, 13);
    lv_obj_set_size(ui->screen_weather_ta_city, 200, 30);

    //Write style for screen_weather_ta_city, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_weather_ta_city, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_ta_city, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_ta_city, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_ta_city, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_ta_city, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_ta_city, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_ta_city, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_ta_city, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_weather_ta_city, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_weather_ta_city, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_weather_ta_city, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_weather_ta_city, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_ta_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_ta_city, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_ta_city, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_ta_city, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_ta_city, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_weather_ta_city, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_weather_ta_city, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_ta_city, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_ta_city, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_ta_city, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_1
    ui->screen_weather_label_1 = lv_label_create(ui->screen_weather_cont_weather);
    lv_label_set_text(ui->screen_weather_label_1, "城市");
    lv_label_set_long_mode(ui->screen_weather_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_weather_label_1, 60, 43);
    lv_obj_set_size(ui->screen_weather_label_1, 100, 32);

    //Write style for screen_weather_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_weather.
    lv_obj_clear_flag(ui->screen_weather, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->screen_weather_cont_weather, LV_OBJ_FLAG_SCROLLABLE);


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_weather);

    //Init events for screen.
    events_init_screen_weather(ui);
}
