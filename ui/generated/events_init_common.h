#ifndef EVENTS_INIT_COMMON_H_
#define EVENTS_INIT_COMMON_H_

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

extern bool is_music_list;

void music_list_show(lv_ui *ui);
void music_list_hide(lv_ui *ui);

#endif