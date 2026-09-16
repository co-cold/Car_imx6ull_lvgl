#include "events_init_common.h"

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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_weather_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, false);
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