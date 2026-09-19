#ifndef __CUSTOM_CARDASHBOARD_H__
#define __CUSTOM_CARDASHBOARD_H__

#include "gui_guider.h"
#include "lvgl.h"

int car_dashboard_init(lv_ui *ui);
void car_dashboard_deinit(void);
void setup_meter_arc_image(lv_ui *ui);

#endif