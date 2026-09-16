#include "events_init_common.h"

static void screen_music_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);

        LOGD("screen_music_event_handler: 进入音乐界面\n");

        custom_media_stop();

        custom_media_init_music();

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
                custom_media_cleanup_screen();
                LOGD("音乐返回主页: 清理完成\n");

                LOGD("音乐返回主页: 加载界面, guider_ui=%p\n", &guider_ui);
                LOGD("音乐返回主页: screen_home=%p\n", &guider_ui.screen_home);
                ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_music_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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
            custom_media_play_song_by_index(0);
            custom_media_start_progress_update();
            custom_media_update_ui_display();
        } else if (state == 1) {
            custom_media_pause();
            custom_media_stop_progress_update();
            custom_media_update_ui_display();
        } else if (state == 2) {
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