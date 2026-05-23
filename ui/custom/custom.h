/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

void custom_init(lv_ui *ui);
void show_control_center();
void hide_control_center();
bool get_ctrl_center_state();
bool get_status_bar_state();
void set_status_bar(uint8_t state);

void slider_set_lable_light(lv_event_t *e);
float slider_update_volume_label(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */