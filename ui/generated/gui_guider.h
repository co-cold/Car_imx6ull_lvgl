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
	lv_obj_t *screen_home_slider_volume;
	lv_obj_t *screen_home_slider_light;
	lv_obj_t *screen_home_label_volume;
	lv_obj_t *screen_home_label_light;
	lv_obj_t *screen_home_btn_settin;
	lv_obj_t *screen_home_btn_settin_label;
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
	lv_obj_t *screen_cam_img_video;
	lv_obj_t *screen_cam_ddlist_device;
	lv_obj_t *screen_weather;
	bool screen_weather_del;
	lv_obj_t *screen_weather_cont_Allweather;
	lv_obj_t *screen_weather_img_9;
	lv_obj_t *screen_weather_img_4;
	lv_obj_t *screen_weather_img_5;
	lv_obj_t *screen_weather_img_6;
	lv_obj_t *screen_weather_img_7;
	lv_obj_t *screen_weather_img_8;
	lv_obj_t *screen_weather_img_10;
	lv_obj_t *screen_weather_img_15;
	lv_obj_t *screen_weather_img_11;
	lv_obj_t *screen_weather_img_12;
	lv_obj_t *screen_weather_img_13;
	lv_obj_t *screen_weather_img_14;
	lv_obj_t *screen_weather_img_21;
	lv_obj_t *screen_weather_img_16;
	lv_obj_t *screen_weather_img_17;
	lv_obj_t *screen_weather_img_20;
	lv_obj_t *screen_weather_img_19;
	lv_obj_t *screen_weather_img_18;
	lv_obj_t *screen_weather_cont_weather;
	lv_obj_t *screen_weather_ta_city;
	lv_obj_t *screen_weather_btn_find;
	lv_obj_t *screen_weather_btn_find_label;
	lv_obj_t *screen_weather_btn_back;
	lv_obj_t *screen_weather_btn_back_label;
	lv_obj_t *screen_weather_cont_part2;
	lv_obj_t *screen_weather_cont_part1;
	lv_obj_t *screen_weather_img_weather;
	lv_obj_t *screen_weather_label_cityName;
	lv_obj_t *screen_weather_label_temDateMin;
	lv_obj_t *screen_weather_label_temDateMax;
	lv_obj_t *screen_weather_label_temDate;
	lv_obj_t *screen_weather_label_weather;
	lv_obj_t *screen_weather_cont_day3;
	lv_obj_t *screen_weather_img_3day;
	lv_obj_t *screen_weather_label_3day;
	lv_obj_t *screen_weather_label_3tem;
	lv_obj_t *screen_weather_label_3weather;
	lv_obj_t *screen_weather_cont_day2;
	lv_obj_t *screen_weather_img_2day;
	lv_obj_t *screen_weather_label_2day;
	lv_obj_t *screen_weather_label_2tem;
	lv_obj_t *screen_weather_label_2weather;
	lv_obj_t *screen_weather_cont_day1;
	lv_obj_t *screen_weather_img_1day;
	lv_obj_t *screen_weather_label_1day;
	lv_obj_t *screen_weather_label_1tem;
	lv_obj_t *screen_weather_label_1weather;
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
	lv_obj_t *screen_music_label_fileName;
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
	lv_obj_t *screen_clock_analog_clock_1;
	lv_obj_t *screen_clock_digital_clock_1;
	lv_obj_t *screen_video;
	bool screen_video_del;
	lv_obj_t *screen_video_cont_video;
	lv_obj_t *screen_video_img_video;
	lv_obj_t *screen_video_cont_videoCtrl;
	lv_obj_t *screen_video_bar_time;
	lv_obj_t *screen_video_btn_playing;
	lv_obj_t *screen_video_btn_playing_label;
	lv_obj_t *screen_video_btn_prev;
	lv_obj_t *screen_video_btn_prev_label;
	lv_obj_t *screen_video_btn_next;
	lv_obj_t *screen_video_btn_next_label;
	lv_obj_t *screen_video_btn_close;
	lv_obj_t *screen_video_btn_close_label;
	lv_obj_t *screen_video_btn_list;
	lv_obj_t *screen_video_btn_list_label;
	lv_obj_t *screen_video_label_fileName;
	lv_obj_t *screen_video_label_progressTime;
	lv_obj_t *screen_video_btn_back;
	lv_obj_t *screen_video_btn_back_label;
	lv_obj_t *screen_video_cont_list;
	lv_obj_t *screen_video_list_video;
	lv_obj_t *screen_video_list_video_item0;
	lv_obj_t *screen_video_list_video_item1;
	lv_obj_t *screen_video_btn_closeList;
	lv_obj_t *screen_video_btn_closeList_label;
	lv_obj_t *screen_carDashboard;
	bool screen_carDashboard_del;
	lv_obj_t *screen_carDashboard_img_carBack;
	lv_obj_t *screen_carDashboard_img_base;
	lv_obj_t *screen_carDashboard_cont_rSpeed;
	lv_obj_t *screen_carDashboard_img_rSpeed;
	lv_obj_t *screen_carDashboard_img_needle;
	lv_obj_t *screen_carDashboard_img_carLog;
	lv_obj_t *screen_carDashboard_label_speedNum1;
	lv_obj_t *screen_carDashboard_label_speedNum2;
	lv_obj_t *screen_carDashboard_label_speedRpm;
	lv_obj_t *screen_carDashboard_meter_rSpeed;
	lv_meter_scale_t *screen_carDashboard_meter_rSpeed_scale_0;
	lv_meter_indicator_t *screen_carDashboard_meter_rSpeed_scale_0_ndline_0;
	lv_meter_indicator_t *screen_carDashboard_meter_rSpeed_scale_0_arc_0;
	lv_obj_t *screen_carDashboard_cont_cSpeed;
	lv_obj_t *screen_carDashboard_img_cSpeed;
	lv_obj_t *screen_carDashboard_img_carLog1;
	lv_obj_t *screen_carDashboard_label_speedNum4;
	lv_obj_t *screen_carDashboard_label_speedNum3;
	lv_obj_t *screen_carDashboard_label_speedSpd;
	lv_obj_t *screen_carDashboard_meter_cSpeed;
	lv_meter_scale_t *screen_carDashboard_meter_cSpeed_scale_0;
	lv_meter_indicator_t *screen_carDashboard_meter_cSpeed_scale_0_ndline_0;
	lv_meter_indicator_t *screen_carDashboard_meter_cSpeed_scale_0_arc_0;
	lv_obj_t *screen_carDashboard_img_line1;
	lv_obj_t *screen_carDashboard_img_line2;
	lv_obj_t *screen_carDashboard_img_line3;
	lv_obj_t *screen_carDashboard_img_line4;
	lv_obj_t *screen_carDashboard_img_cen1;
	lv_obj_t *screen_carDashboard_label_largeNum;
	lv_obj_t *screen_carDashboard_label_largeU;
	lv_obj_t *screen_carDashboard_img_arc1;
	lv_obj_t *screen_carDashboard_img_arc2;
	lv_obj_t *screen_carDashboard_cont_bt;
	lv_obj_t *screen_carDashboard_cont_tpR;
	lv_obj_t *screen_carDashboard_img_AbsOff;
	lv_obj_t *screen_carDashboard_img_Door;
	lv_obj_t *screen_carDashboard_img_Seatbelt;
	lv_obj_t *screen_carDashboard_img_AbsOn;
	lv_obj_t *screen_carDashboard_cont_btL;
	lv_obj_t *screen_carDashboard_img_ACC;
	lv_obj_t *screen_carDashboard_img_HighBeam;
	lv_obj_t *screen_carDashboard_cont_btR;
	lv_obj_t *screen_carDashboard_img_RearFog;
	lv_obj_t *screen_carDashboard_img_FrontFog;
	lv_obj_t *screen_carDashboard_cont_tpL;
	lv_obj_t *screen_carDashboard_img_turnL;
	lv_obj_t *screen_carDashboard_img_turnR;
	lv_obj_t *screen_carDashboard_img_car1;
	lv_obj_t *screen_map;
	bool screen_map_del;
	lv_obj_t *screen_map_imgbtn_map;
	lv_obj_t *screen_map_imgbtn_map_label;
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
void setup_scr_screen_video(lv_ui *ui);
void setup_scr_screen_carDashboard(lv_ui *ui);
void setup_scr_screen_map(lv_ui *ui);
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

LV_IMG_DECLARE(_musicBack2l1_800x480);
LV_IMG_DECLARE(_weather_3_alpha_100x100);
LV_IMG_DECLARE(_weather_18_alpha_100x100);
LV_IMG_DECLARE(_weather_2_alpha_100x100);
LV_IMG_DECLARE(_weather_22_alpha_100x100);
LV_IMG_DECLARE(_weather_13_alpha_100x100);
LV_IMG_DECLARE(_weather_24_alpha_100x100);
LV_IMG_DECLARE(_weather_4_alpha_100x100);
LV_IMG_DECLARE(_weather_18_alpha_150x150);
LV_IMG_DECLARE(_weather_7_alpha_100x100);
LV_IMG_DECLARE(_weather_1_alpha_150x150);
LV_IMG_DECLARE(_weather_13_alpha_150x150);
LV_IMG_DECLARE(_weather_12_alpha_150x150);
LV_IMG_DECLARE(_weather_7_alpha_150x150);
LV_IMG_DECLARE(_weather_2_alpha_150x150);
LV_IMG_DECLARE(_weather_22_alpha_150x150);
LV_IMG_DECLARE(_weather_4_alpha_150x150);
LV_IMG_DECLARE(_weather_3_alpha_150x150);
LV_IMG_DECLARE(_weather_24_alpha_150x150);

LV_IMG_DECLARE(_weatherBK_800x480);
LV_IMG_DECLARE(_weather_0_alpha_150x150);
LV_IMG_DECLARE(_weather_2_alpha_100x100);
LV_IMG_DECLARE(_weather_12_alpha_100x100);
LV_IMG_DECLARE(_weather_1_alpha_100x100);

LV_IMG_DECLARE(_musicBack2l1_800x480);
LV_IMG_DECLARE(_1725420839434_alpha_250x250);

LV_IMG_DECLARE(_musicBack2l1_800x300);
LV_IMG_DECLARE(_noVideo_alpha_800x450);
LV_IMG_DECLARE(_musicBack2l1_alpha_800x480);
LV_IMG_DECLARE(_Base_alpha_800x400);
LV_IMG_DECLARE(_gauge_bg_alpha_250x250);
LV_IMG_DECLARE(_gauge_bgL_alpha_250x250);
LV_IMG_DECLARE(_gauge_needle_alpha_94x12);
LV_IMG_DECLARE(_car_icon_alpha_75x15);
LV_IMG_DECLARE(_gauge_needle_alpha_94x12);
LV_IMG_DECLARE(_gauge_bg_alpha_250x250);
LV_IMG_DECLARE(_car_icon_alpha_75x15);
LV_IMG_DECLARE(_gauge_needle_alpha_94x12);
LV_IMG_DECLARE(_line1_alpha_141x266);
LV_IMG_DECLARE(_line2_alpha_141x266);
LV_IMG_DECLARE(_line3_alpha_183x340);
LV_IMG_DECLARE(_line4_alpha_183x340);
LV_IMG_DECLARE(_cen1_alpha_56x300);
LV_IMG_DECLARE(_arc1_alpha_362x17);
LV_IMG_DECLARE(_arc2_alpha_362x17);
LV_IMG_DECLARE(_ABS0_alpha_55x45);
LV_IMG_DECLARE(_Door_alpha_45x45);
LV_IMG_DECLARE(_Seatbelt_alpha_45x45);
LV_IMG_DECLARE(_ABS1_alpha_55x45);
LV_IMG_DECLARE(_ACC_alpha_45x40);
LV_IMG_DECLARE(_HighBeam_alpha_42x35);
LV_IMG_DECLARE(_RearFog_alpha_45x40);
LV_IMG_DECLARE(_FrontFog_alpha_45x40);
LV_IMG_DECLARE(_TurnSignalL_alpha_60x55);
LV_IMG_DECLARE(_TurnSignalR_alpha_60x55);
LV_IMG_DECLARE(_car1_alpha_100x100);
LV_IMG_DECLARE(_weatherBK_alpha_800x480);

LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_40)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_20)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_14)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_25)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_30)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_24)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_12)
LV_FONT_DECLARE(lv_font_Acme_Regular_25)
LV_FONT_DECLARE(lv_font_Acme_Regular_12)
LV_FONT_DECLARE(lv_font_Acme_Regular_145)
LV_FONT_DECLARE(lv_font_Acme_Regular_35)
LV_FONT_DECLARE(lv_font_montserratMedium_58)
LV_FONT_DECLARE(lv_font_montserratMedium_12)


#ifdef __cplusplus
}
#endif
#endif
