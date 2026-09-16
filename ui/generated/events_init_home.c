#include "events_init_common.h"

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
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_carDashboard, guider_ui.screen_carDashboard_del, &guider_ui.screen_home_del, setup_scr_screen_carDashboard, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, false, false);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_video, guider_ui.screen_video_del, &guider_ui.screen_home_del, setup_scr_screen_video, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_music, guider_ui.screen_music_del, &guider_ui.screen_home_del, setup_scr_screen_music, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_weather, guider_ui.screen_weather_del, &guider_ui.screen_home_del, setup_scr_screen_weather, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_clock, guider_ui.screen_clock_del, &guider_ui.screen_home_del, setup_scr_screen_clock, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_cam, guider_ui.screen_cam_del, &guider_ui.screen_home_del, setup_scr_screen_cam, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
        LOGD("初始化摄像头...\n");
        camera_ui_t *camera = camera_ui_init(guider_ui.screen_cam_img_video,
                                           "/dev/video1", 640, 480, 30);
        if (!camera) {
            LOGD("摄像头初始化失败\n");
            return;
        }

        LOGD("启动摄像头...\n");
        if (camera_ui_start(camera) < 0) {
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