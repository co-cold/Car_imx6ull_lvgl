#include "events_init_common.h"

static void screen_cam_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        camera_ui_deinit(NULL);
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
        LOGD("停止摄像头...\n");
        camera_ui_stop(NULL);

        LOGD("反初始化摄像头...\n");
        camera_ui_deinit(NULL);

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