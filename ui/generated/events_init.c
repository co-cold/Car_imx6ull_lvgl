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
#include <string.h>
#include <stdlib.h>
#include "lvgl.h"
#include "custom_media.h"
#include "custom_font.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


bool is_music_list = false;
void music_list_show(lv_ui *ui)
{
    is_music_list = true;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui->screen_music_cont_list);
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
    is_music_list = false;
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
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_carDashboard, guider_ui.screen_carDashboard_del, &guider_ui.screen_home_del, setup_scr_screen_carDashboard, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, false, true);
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

static void screen_home_slider_volume_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        float volume = slider_update_volume_label(e);
        custom_media_set_volume(volume);
        break;
    }
    default:
        break;
    }
}

static void screen_home_slider_light_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        slider_set_lable_light(e);
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

static void screen_home_btn_video_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("打开视频\n");
        custom_media_init(&guider_ui);
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_video, guider_ui.screen_video_del, &guider_ui.screen_home_del, setup_scr_screen_video, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
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
        LOGD("打开音乐\n");
        custom_media_init(&guider_ui);
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
        LOGD("打开天气\n");
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
        LOGD("打开时钟\n");
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
        // 1. 初始化摄像头
        LOGD("初始化摄像头...\n");
        camera_ui_t *camera = camera_ui_init(guider_ui.screen_cam_img_video, 
                                           "/dev/video1", 640, 480, 30);
        if (!camera) {
            LOGD("摄像头初始化失败\n");
            return;
        }
        
        // 2. 启动摄像头
        LOGD("启动摄像头...\n");
        if (camera_ui_start(camera) < 0) {  // 或者使用camera_ui_start(NULL)
            LOGD("摄像头启动失败\n");
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
    lv_obj_add_event_cb(ui->screen_home_slider_volume, screen_home_slider_volume_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_slider_light, screen_home_slider_light_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_cont_touch, screen_home_cont_touch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_home_btn_video, screen_home_btn_video_event_handler, LV_EVENT_ALL, ui);
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
        LOGD("停止摄像头...\n");
        camera_ui_stop(NULL);  // 使用全局实例
        
        // 2. 反初始化摄像头
        LOGD("反初始化摄像头...\n");
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
        break;
    }
    default:
        break;
    }
}

static void screen_weather_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_weather_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_weather (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_weather, screen_weather_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_weather_btn_back, screen_weather_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_music_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        
        LOGD("screen_music_event_handler: 进入音乐界面\n");
        
        // 强制停止所有播放
        custom_media_stop();
        
        // 初始化音乐功能（设置模式并扫描音乐目录）
        custom_media_init_music();
        
        // 初始化音乐列表 UI
        LOGD("screen_music_event_handler: 初始化音乐列表\n");
        custom_media_init_music_list(&guider_ui);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            if (!is_music_list) {
                LOGD("音乐返回主页\n");
                custom_media_deinit();
                LOGD("音乐返回主页: 反初始化完成\n");

                LOGD("音乐返回主页: 加载界面, guider_ui=%p\n", &guider_ui);
                LOGD("音乐返回主页: screen_home=%p\n", &guider_ui.screen_home);
                ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_music_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
                LOGD("音乐返回主页: 界面加载完成\n");
            }
            break;
        }
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

static void screen_music_btn_next_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        int state = custom_media_get_state();
        if (state == 0) {
            custom_media_play_song_by_index(custom_media_get_current_song_idx());
        } else {
            custom_media_play_next();
        }
        custom_media_start_progress_update();
        custom_media_update_ui_display();
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
        int state = custom_media_get_state();
        if (state == 0) {
            custom_media_play_song_by_index(custom_media_get_current_song_idx());
        } else {
            custom_media_play_prev();
        }
        custom_media_start_progress_update();
        custom_media_update_ui_display();
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
        int state = custom_media_get_state();
        
        if (state == 0) {
            // 未播放，开始播放第一首歌
            custom_media_play_song_by_index(0);
            custom_media_start_progress_update();
            custom_media_update_ui_display();
        } else if (state == 1) {
            // 正在播放，暂停
            custom_media_pause();
            custom_media_stop_progress_update();
            custom_media_update_ui_display();
        } else if (state == 2) {
            // 已暂停，恢复
            custom_media_resume();
            custom_media_start_progress_update();
            custom_media_update_ui_display();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_music_slider_progressTime_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t *slider = lv_event_get_target(e);
        int progress = lv_slider_get_value(slider);
        
        double duration = custom_media_get_duration();
        if (duration > 0) {
            double pos = (progress / 100.0) * duration;
            custom_media_seek(pos);
        }
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
    lv_obj_add_event_cb(ui->screen_music_slider_progressTime, screen_music_slider_progressTime_event_handler, LV_EVENT_ALL, ui);
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

static void screen_video_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        
        LOGD("screen_video_event_handler: 进入视频界面\n");

        // 初始化视频功能（设置模式并扫描视频目录）
        custom_media_init_video();
        
        // 初始化视频列表 UI
        LOGD("screen_video_event_handler: 初始化视频列表\n");
        custom_media_init_video_list(&guider_ui);
        
        LOGD("screen_video_event_handler: 视频帧更新定时器由 custom_media 管理\n");
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        // 视频帧更新定时器由 custom_media_deinit() 管理
        break;
    }
    default:
        break;
    }
}

static void screen_video_img_video_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        // 显示控制栏
        lv_obj_clear_flag(guider_ui.screen_video_cont_videoCtrl, LV_OBJ_FLAG_HIDDEN);

        break;
    }
    default:
        break;
    }
}

static void screen_video_cont_videoCtrl_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.screen_video_cont_videoCtrl, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_list_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("screen_video_btn_list_event_handler: 收到 CLICKED 事件\n");
        lv_obj_clear_flag(guider_ui.screen_video_cont_list, LV_OBJ_FLAG_HIDDEN);
        LOGD("screen_video_btn_list_event_handler: 显示视频列表\n");
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_playing_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("screen_video_btn_playing_event_handler: 收到 CLICKED 事件\n");
        int state = custom_media_get_state();
        LOGD("screen_video_btn_playing_event_handler: 状态=%d\n", state);
        
        if (state == 0) {
            // 未播放，开始播放第一首视频
            LOGD("screen_video_btn_playing_event_handler: 播放视频索引 0\n");
            int result = custom_media_play_video_by_index(0);
            LOGD("screen_video_btn_playing_event_handler: 播放结果=%d\n", result);
            lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PAUSE);
            LOGD("screen_video_btn_playing_event_handler: 标签更新为暂停\n");
        } else if (state == 1) {
            // 正在播放，暂停
            LOGD("screen_video_btn_playing_event_handler: 暂停中\n");
            custom_media_pause();
            lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PLAY);
            LOGD("screen_video_btn_playing_event_handler: 标签更新为播放\n");
        } else if (state == 2) {
            // 已暂停，恢复
            LOGD("screen_video_btn_playing_event_handler: 恢复中\n");
            custom_media_resume();
            lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PAUSE);
            LOGD("screen_video_btn_playing_event_handler: label updated to PAUSE\n");
        }
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_prev_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("screen_video_btn_prev_event_handler: 收到 CLICKED 事件\n");
        LOGD("screen_video_btn_prev_event_handler: 当前状态=%d\n", custom_media_get_state());
        // 播放上一个视频（使用视频专用函数）
        int result = custom_media_play_prev_video();
        LOGD("screen_video_btn_prev_event_handler: 播放上一首结果=%d\n", result);
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_next_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("screen_video_btn_next_event_handler: 收到 CLICKED 事件\n");
        LOGD("screen_video_btn_next_event_handler: 当前状态=%d\n", custom_media_get_state());
        // 播放下一个视频（使用视频专用函数）
        int result = custom_media_play_next_video();
        LOGD("screen_video_btn_next_event_handler: 播放下一首结果=%d\n", result);
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("视频返回主页: 开始\n");
        custom_media_deinit();
        LOGD("视频返回主页: deinit done\n");
        
        // 处理所有待处理的 LVGL 任务
        LOGD("视频返回主页: 处理 LVGL 任务...\n");
        for (int i = 0; i < 10; i++) {
            lv_task_handler();
            usleep(10000); // 10ms
        }
        LOGD("视频返回主页: LVGL 任务处理完成\n");
        
        LOGD("视频返回主页: 开始屏幕切换...\n");
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_video_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        LOGD("视频返回主页: 屏幕切换完成\n");
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_close_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        LOGD("screen_video_btn_close_event_handler: 收到 CLICKED 事件\n");
        
        // 停止当前视频播放（不退出界面）
        custom_media_stop();
        
        // 更新播放按钮状态为播放图标
        lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PLAY);
        
        // 清空视频显示区域
        if (guider_ui.screen_video_img_video) {
            lv_img_set_src(guider_ui.screen_video_img_video, LV_SYMBOL_VIDEO);
            lv_obj_invalidate(guider_ui.screen_video_img_video);
        }
        
        // 清空文件名显示
        if (guider_ui.screen_video_label_fileName) {
            lv_label_set_text(guider_ui.screen_video_label_fileName, "");
        }
        
        // 重置进度条
        if (guider_ui.screen_video_bar_time) {
            lv_slider_set_value(guider_ui.screen_video_bar_time, 0, LV_ANIM_OFF);
        }
        
        LOGD("screen_video_btn_close_event_handler: 视频已关闭\n");
        break;
    }
    default:
        break;
    }
}

static void screen_video_btn_closeList_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        // 隐藏列表
        lv_obj_add_flag(guider_ui.screen_video_cont_list, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_video (lv_ui *ui)
{
    LOGD("events_init_screen_video: 调用, ui=%p\n", ui);
    
    if (!ui) {
        LOGD("events_init_screen_video: 错误！ui 为空\n");
        return;
    }
    
    lv_obj_add_event_cb(ui->screen_video, screen_video_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_img_video, screen_video_img_video_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_cont_videoCtrl, screen_video_cont_videoCtrl_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_list, screen_video_btn_list_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_back, screen_video_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_closeList, screen_video_btn_closeList_event_handler, LV_EVENT_ALL, ui);
    
    // 添加视频控制按键事件处理
    lv_obj_add_event_cb(ui->screen_video_btn_playing, screen_video_btn_playing_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_prev, screen_video_btn_prev_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_next, screen_video_btn_next_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_close, screen_video_btn_close_event_handler, LV_EVENT_ALL, ui);
}

static void screen_carDashboard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        start_dashboard_animation(&guider_ui);
        set_status_bar(0);
        //event_carDashboard
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        stop_dashboard_animation();
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_carDashboard_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 200, false, true);
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

void events_init_screen_carDashboard (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_carDashboard, screen_carDashboard_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}