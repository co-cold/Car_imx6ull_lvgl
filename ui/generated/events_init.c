/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


void music_list_show(lv_ui *ui)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui->screen_music_cont_list); // ✅ 动的是容器
    lv_anim_set_values(&a, 480, 180);
    lv_anim_set_time(&a, 300);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}
void music_list_hide(lv_ui *ui)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui->screen_music_cont_list);
    lv_anim_set_values(&a, 180, 480);   // 从 180 → 滑出屏幕
    lv_anim_set_time(&a, 300);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
}

static void screen_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(1);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_BOTTOM:
        {
            lv_indev_wait_release(lv_indev_get_act());
            show_control_center();
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_home_cont_touch_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        hide_control_center();
        break;
    }
    default:
        break;
    }
}

static void screen_home_btn_music_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_music, guider_ui.screen_music_del, &guider_ui.screen_home_del, setup_scr_screen_music, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_home_btn_weather_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_weather, guider_ui.screen_weather_del, &guider_ui.screen_home_del, setup_scr_screen_weather, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_home_btn_clock_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_clock, guider_ui.screen_clock_del, &guider_ui.screen_home_del, setup_scr_screen_clock, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_home_btn_cam_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_cam, guider_ui.screen_cam_del, &guider_ui.screen_home_del, setup_scr_screen_cam, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        // 2. 初始化摄像头
        printf("初始化摄像头...\n");
        camera_ui_t *camera = camera_ui_init(guider_ui.screen_cam_img_video, 
                                           "/dev/video1", 640, 480, 30);
        if (!camera) {
            printf("摄像头初始化失败\n");
            return;
        }
        
        // 3. 启动摄像头
        printf("启动摄像头...\n");
        if (camera_ui_start(camera) < 0) {  // 或者使用camera_ui_start(NULL)
            printf("摄像头启动失败\n");
            return;
        }
        
        break;
    }
    default:
        break;
    }
}

void events_init_screen_home (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_home, screen_home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_cont_touch, screen_home_cont_touch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_btn_music, screen_home_btn_music_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_btn_weather, screen_home_btn_weather_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_btn_clock, screen_home_btn_clock_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_btn_cam, screen_home_btn_cam_event_handler, LV_EVENT_ALL, ui);
}

static void screen_cam_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_BOTTOM:
        {
            lv_indev_wait_release(lv_indev_get_act());
            show_control_center();
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_cam_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        // 1. 停止摄像头
        printf("停止摄像头...\n");
        camera_ui_stop(NULL);  // 使用全局实例
        
        // 2. 反初始化摄像头
        printf("反初始化摄像头...\n");
        camera_ui_deinit(NULL);  // 使用全局实例

        ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_cam_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_cam (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_cam, screen_cam_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_cam_btn_close, screen_cam_btn_close_event_handler, LV_EVENT_ALL, ui);
}

static void screen_weather_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_weather_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_weather (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_weather, screen_weather_event_handler, LV_EVENT_ALL, ui);
}

static void screen_music_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_music_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_music_btn_next_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_music_btn_prev_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_music_btn_playing_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_music_btn_list_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        music_list_show(&guider_ui);
        break;
    }
    default:
        break;
    }
}

static void screen_music_btn_closeList_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_list_hide(&guider_ui);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_music (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_music, screen_music_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_music_btn_next, screen_music_btn_next_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_music_btn_prev, screen_music_btn_prev_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_music_btn_playing, screen_music_btn_playing_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_music_btn_list, screen_music_btn_list_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_music_btn_closeList, screen_music_btn_closeList_event_handler, LV_EVENT_ALL, ui);
}

static void screen_clock_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_clock_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_clock (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_clock, screen_clock_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
