#include "events_init_common.h"

static void screen_video_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);

        LOGD("screen_video_event_handler: 进入视频界面\n");

        custom_media_init_video();

        LOGD("screen_video_event_handler: 初始化视频列表\n");
        custom_media_init_video_list(&guider_ui);

        LOGD("screen_video_event_handler: 视频帧更新定时器由 custom_media 管理\n");
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
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
            LOGD("screen_video_btn_playing_event_handler: 播放视频索引 0\n");
            int result = custom_media_play_video_by_index(0);
            LOGD("screen_video_btn_playing_event_handler: 播放结果=%d\n", result);
            lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PAUSE);
            LOGD("screen_video_btn_playing_event_handler: 标签更新为暂停\n");
        } else if (state == 1) {
            LOGD("screen_video_btn_playing_event_handler: 暂停中\n");
            custom_media_pause();
            lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PLAY);
            LOGD("screen_video_btn_playing_event_handler: 标签更新为播放\n");
        } else if (state == 2) {
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
        custom_media_cleanup_screen();
        LOGD("视频返回主页: cleanup done\n");

        LOGD("视频返回主页: 处理 LVGL 任务...\n");
        for (int i = 0; i < 10; i++) {
            lv_task_handler();
            usleep(10000);
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

        custom_media_stop();

        lv_label_set_text(guider_ui.screen_video_btn_playing_label, LV_SYMBOL_PLAY);

        if (guider_ui.screen_video_img_video) {
            lv_img_set_src(guider_ui.screen_video_img_video, &_noVideo_alpha_800x450);
            lv_obj_invalidate(guider_ui.screen_video_img_video);
        }

        if (guider_ui.screen_video_label_fileName) {
            lv_label_set_text(guider_ui.screen_video_label_fileName, "");
        }

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

    lv_obj_add_event_cb(ui->screen_video_btn_playing, screen_video_btn_playing_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_prev, screen_video_btn_prev_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_next, screen_video_btn_next_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_video_btn_close, screen_video_btn_close_event_handler, LV_EVENT_ALL, ui);
}