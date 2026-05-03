/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen_home;
	bool screen_home_del;
	lv_obj_t *screen_home_cont_control_center;
	lv_obj_t *screen_home_slider_2;
	lv_obj_t *screen_home_slider_1;
	lv_obj_t *screen_home_btn_13;
	lv_obj_t *screen_home_btn_13_label;
	lv_obj_t *screen_home_btn_12;
	lv_obj_t *screen_home_btn_12_label;
	lv_obj_t *screen_home_btn_11;
	lv_obj_t *screen_home_btn_11_label;
	lv_obj_t *screen_home_btn_10;
	lv_obj_t *screen_home_btn_10_label;
	lv_obj_t *screen_home_btn_9;
	lv_obj_t *screen_home_btn_9_label;
	lv_obj_t *screen_home_btn_8;
	lv_obj_t *screen_home_btn_8_label;
	lv_obj_t *screen_home_btn_7;
	lv_obj_t *screen_home_btn_7_label;
	lv_obj_t *screen_home_btn_6;
	lv_obj_t *screen_home_btn_6_label;
	lv_obj_t *screen_home_btn_5;
	lv_obj_t *screen_home_btn_5_label;
	lv_obj_t *screen_home_btn_4;
	lv_obj_t *screen_home_btn_4_label;
	lv_obj_t *screen_home_btn_3;
	lv_obj_t *screen_home_btn_3_label;
	lv_obj_t *screen_home_btn_2;
	lv_obj_t *screen_home_btn_2_label;
	lv_obj_t *screen_home_btn_1;
	lv_obj_t *screen_home_btn_1_label;
	lv_obj_t *screen_home_cont_touch;
	lv_obj_t *screen_home_cont_home;
	lv_obj_t *screen_home_img_background;
	lv_obj_t *screen_home_tileview_app;
	lv_obj_t *screen_home_tileview_app_tile1;
	lv_obj_t *screen_home_tileview_app_tile2;
	lv_obj_t *screen_home_btn_video;
	lv_obj_t *screen_home_btn_video_label;
	lv_obj_t *screen_home_btn_music;
	lv_obj_t *screen_home_btn_music_label;
	lv_obj_t *screen_home_btn_weather;
	lv_obj_t *screen_home_btn_weather_label;
	lv_obj_t *screen_home_btn_calculator;
	lv_obj_t *screen_home_btn_calculator_label;
	lv_obj_t *screen_home_btn_setting;
	lv_obj_t *screen_home_btn_setting_label;
	lv_obj_t *screen_home_btn_pictrue;
	lv_obj_t *screen_home_btn_pictrue_label;
	lv_obj_t *screen_home_btn_calendar;
	lv_obj_t *screen_home_btn_calendar_label;
	lv_obj_t *screen_home_btn_clock;
	lv_obj_t *screen_home_btn_clock_label;
	lv_obj_t *screen_home_btn_notes;
	lv_obj_t *screen_home_btn_notes_label;
	lv_obj_t *screen_home_btn_recoder;
	lv_obj_t *screen_home_btn_recoder_label;
	lv_obj_t *screen_home_btn_ai;
	lv_obj_t *screen_home_btn_ai_label;
	lv_obj_t *screen_home_cont_docker;
	lv_obj_t *screen_home_btn_cam;
	lv_obj_t *screen_home_btn_cam_label;
	lv_obj_t *screen_home_cont_status_bar;
	lv_obj_t *screen_home_label_status_bt;
	lv_obj_t *screen_home_label_status_eq;
	lv_obj_t *screen_home_label_status_wifi;
	lv_obj_t *screen_home_digital_clock_status;
	lv_obj_t *screen_cam;
	bool screen_cam_del;
	lv_obj_t *screen_cam_cont_cam;
	lv_obj_t *screen_cam_btn_close;
	lv_obj_t *screen_cam_btn_close_label;
	lv_obj_t *screen_cam_btn_1;
	lv_obj_t *screen_cam_btn_1_label;
	lv_obj_t *screen_cam_img_video;
	lv_obj_t *screen_weather;
	bool screen_weather_del;
	lv_obj_t *screen_weather_cont_weather;
	lv_obj_t *screen_weather_ta_city;
	lv_obj_t *screen_weather_label_1;
	lv_obj_t *screen_music;
	bool screen_music_del;
	lv_obj_t *screen_music_cont_music;
	lv_obj_t *screen_music_img_music;
	lv_obj_t *screen_music_btn_next;
	lv_obj_t *screen_music_btn_next_label;
	lv_obj_t *screen_music_btn_prev;
	lv_obj_t *screen_music_btn_prev_label;
	lv_obj_t *screen_music_btn_playing;
	lv_obj_t *screen_music_btn_playing_label;
	lv_obj_t *screen_music_btn_list;
	lv_obj_t *screen_music_btn_list_label;
	lv_obj_t *screen_music_line_1;
	lv_obj_t *screen_music_slider_progressTime;
	lv_obj_t *screen_music_label_lyric;
	lv_obj_t *screen_music_label_progressTime;
	lv_obj_t *screen_music_label_endTime;
	lv_obj_t *screen_music_label_singer;
	lv_obj_t *screen_music_label_song;
	lv_obj_t *screen_music_cont_list;
	lv_obj_t *screen_music_list_music;
	lv_obj_t *screen_music_list_music_item0;
	lv_obj_t *screen_music_list_music_item1;
	lv_obj_t *screen_music_list_music_item2;
	lv_obj_t *screen_music_btn_closeList;
	lv_obj_t *screen_music_btn_closeList_label;
	lv_obj_t *screen_clock;
	bool screen_clock_del;
	lv_obj_t *screen_clock_cont_clock;
	lv_obj_t *screen_clock_label_1;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen_home(lv_ui *ui);
void setup_scr_screen_cam(lv_ui *ui);
void setup_scr_screen_weather(lv_ui *ui);
void setup_scr_screen_music(lv_ui *ui);
void setup_scr_screen_clock(lv_ui *ui);
LV_IMG_DECLARE(_1694234191268_alpha_800x480);

LV_IMG_DECLARE(_videoplayer_100x100);

LV_IMG_DECLARE(_music_100x100);

LV_IMG_DECLARE(_weather_100x100);

LV_IMG_DECLARE(_calculator2_100x100);

LV_IMG_DECLARE(_settings_100x100);

LV_IMG_DECLARE(_gallery3d_100x100);

LV_IMG_DECLARE(_calendar_100x100);

LV_IMG_DECLARE(_deskclock_100x100);

LV_IMG_DECLARE(_notes_100x100);

LV_IMG_DECLARE(_soundrecorder_100x100);

LV_IMG_DECLARE(_voicedialer_100x100);

LV_IMG_DECLARE(_camera_100x100);

LV_IMG_DECLARE(_weatherBK_800x480);
LV_IMG_DECLARE(_1725420839434_alpha_250x250);

LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_15)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_40)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_20)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_12)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)


#ifdef __cplusplus
}
#endif
#endif
