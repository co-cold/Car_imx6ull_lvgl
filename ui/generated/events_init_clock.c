#include "events_init_common.h"

static void screen_clock_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        set_status_bar(0);
        break;
    }
    case LV_EVENT_SCREEN_LOADED:
    {
        custom_set_Aclock(guider_ui.screen_clock_analog_clock_1);
        custom_set_Dclock(guider_ui.screen_clock_digital_clock_1);
        custom_set_Ctimer();
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        custom_close_Ctimer();
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_clock_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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