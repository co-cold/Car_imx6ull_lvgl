#include "events_init_common.h"
#include "custom_carDashboard.h"

static void screen_carDashboard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        car_dashboard_init(&guider_ui);
        set_status_bar(0);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        car_dashboard_deinit();
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_carDashboard_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 200, false, false);
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