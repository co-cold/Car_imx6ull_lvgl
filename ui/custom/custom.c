/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "custom_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
static lv_obj_t *ctrl_center = NULL;
static lv_obj_t *status_bar = NULL;
static lv_obj_t *Gclock = NULL;
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
    custom_font_init("/home/debian/font/SourceHanSerifSC-Regular.otf", 16);
}

void custom_top_init(lv_ui *ui)
{
    /* 1. 处理控制栏 */
    if (ctrl_center == NULL) {
        // 第一次进入：保存新创建的对象
        ctrl_center = ui->screen_home_cont_control_center;
    } else {
        // 非第一次进入：隐藏新创建的对象，显示旧对象
        lv_obj_add_flag(ui->screen_home_cont_control_center, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ctrl_center, LV_OBJ_FLAG_HIDDEN);
        // 更新 ui 结构体指针，指向旧控制栏
        ui->screen_home_cont_control_center = ctrl_center;
    }
    lv_obj_set_parent(ctrl_center, lv_layer_top());

    /* 2. 处理状态栏 + 时钟 */
    if (status_bar == NULL) {
        // 第一次进入：保存状态栏和时钟
        status_bar = ui->screen_home_cont_status_bar;
        Gclock = ui->screen_home_digital_clock_status;
    } else {
        // 非第一次进入：隐藏新创建的状态栏，显示旧状态栏
        lv_obj_add_flag(ui->screen_home_cont_status_bar, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
        // 更新 ui 结构体指针，指向旧状态栏和时钟
        ui->screen_home_cont_status_bar = status_bar;
        ui->screen_home_digital_clock_status = Gclock;
    }
    lv_obj_set_parent(status_bar, lv_layer_top());
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN); // 确保显示
    
    // 确保时钟依附于复用的状态栏
    if (lv_obj_is_valid(Gclock) && lv_obj_is_valid(status_bar)) {
        lv_obj_set_parent(Gclock, status_bar);
    }
}

void show_control_center()
{
    if (!ctrl_center) return;
    
    lv_obj_clear_flag(ctrl_center, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(ctrl_center);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ctrl_center);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, -480, 0);
    lv_anim_set_time(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void hide_control_center()
{
    if (!ctrl_center) return;
  
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ctrl_center);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, 0, -480);
    lv_anim_set_time(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
}

void set_status_bar(uint8_t state)
{
    if(status_bar == NULL){return;}
    if(state == 1) {
        lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
    } else if(state == 0) {
        lv_obj_add_flag(status_bar, LV_OBJ_FLAG_HIDDEN);
    }
}

void slider_set_lable_light(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider) return;
    
    int32_t value = lv_slider_get_value(slider);
    
    // 获取滑块的关联标签
    lv_obj_t *label = lv_obj_get_user_data(slider);
    if (label) {
        lv_label_set_text_fmt(label, "%d%%", value);
    }
}

float slider_update_volume_label(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider) return 0.0f;
    
    // 获取滑块值并更新标签
    int32_t value = lv_slider_get_value(slider);
    lv_obj_t *label = lv_obj_get_user_data(slider);
    if (label) {
        lv_label_set_text_fmt(label, "%d%%", value);
    }
    
    // 转换为音量值 (0.0 - 1.0)，滑块值直接对应百分比
    float volume = value / 100.0f;
    return volume;
}



/**********************
 * 静态变量与常量
 **********************/
static lv_anim_t rpm_anim, spd_anim;
static lv_timer_t *blink_timer = NULL;
static bool anim_running = false, blink_phase = false;

/* 指示灯位定义 */
typedef enum {
    IND_TURN_L = 1<<0, IND_TURN_R = 1<<1, IND_FRONT_FOG = 1<<2,
    IND_REAR_FOG = 1<<3, IND_HIGH_BEAM = 1<<4, IND_ABS = 1<<5,
    IND_ACC = 1<<6, IND_SEATBELT = 1<<7
} ind_bit_t;
static uint8_t ind_state = 0;

/* 公共工具函数 */
static int32_t clamp_arc(int32_t v) {
    return v < 8 ? 8 : (v > 108 ? 108 : v);
}

static void set_ind(ind_bit_t bit, bool active) {
    active ? (ind_state |= bit) : (ind_state &= ~bit);
}

/* 指示灯映射表 */
typedef struct { lv_obj_t **obj; ind_bit_t bit; } ind_map_t;
#define IND_MAP(obj, bit) {&(ui->screen_carDashboard_img_##obj), bit}

/**********************
 * 指示灯闪烁回调（仅处理闪烁组）
 **********************/
static void blink_cb(lv_timer_t *t) {
    lv_ui *ui = t->user_data;
    blink_phase = !blink_phase;
    
    /* 闪烁组：转向灯、雾灯、远光灯 */
    ind_map_t blink_maps[] = {
        IND_MAP(turnL, IND_TURN_L), IND_MAP(turnR, IND_TURN_R),
        IND_MAP(FrontFog, IND_FRONT_FOG), IND_MAP(RearFog, IND_REAR_FOG),
        IND_MAP(HighBeam, IND_HIGH_BEAM)
    };
    for (size_t i = 0; i < sizeof(blink_maps)/sizeof(blink_maps[0]); i++) {
        if (*blink_maps[i].obj && (ind_state & blink_maps[i].bit)) {
            blink_phase ? 
                lv_obj_clear_flag(*blink_maps[i].obj, LV_OBJ_FLAG_HIDDEN) :
                lv_obj_add_flag(*blink_maps[i].obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    /* 常亮组：安全带、ACC */
    ind_map_t non_blink_maps[] = {
        IND_MAP(Seatbelt, IND_SEATBELT), IND_MAP(ACC, IND_ACC)
    };
    for (size_t i = 0; i < sizeof(non_blink_maps)/sizeof(non_blink_maps[0]); i++) {
        if (*non_blink_maps[i].obj) {
            (ind_state & non_blink_maps[i].bit) ?
                lv_obj_clear_flag(*non_blink_maps[i].obj, LV_OBJ_FLAG_HIDDEN) :
                lv_obj_add_flag(*non_blink_maps[i].obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    /* ABS：故障时常亮（按需调整） */
    if (ui->screen_carDashboard_img_AbsOn && ui->screen_carDashboard_img_AbsOff) {
        if (ind_state & IND_ABS) {
            lv_obj_clear_flag(ui->screen_carDashboard_img_AbsOff, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui->screen_carDashboard_img_AbsOn, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ui->screen_carDashboard_img_AbsOff, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui->screen_carDashboard_img_AbsOn, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/**********************
 * 转速表回调
 **********************/
static void set_rpm(void *var, int32_t v) {
    if (!anim_running) return;
    lv_ui *ui = var;
    if (!ui || !ui->screen_carDashboard_meter_rSpeed) return;
    
    if (ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0)
        lv_meter_set_indicator_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0, v);
    
    if (ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0)
        lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0, clamp_arc(v));
    
    if (ui->screen_carDashboard_label_speedNum1) {
        char buf[8];
        sprintf(buf, "%d", (v-8)*100);
        lv_label_set_text(ui->screen_carDashboard_label_speedNum1, buf);
    }
}

/**********************
 * 车速表回调
 **********************/
static void set_spd(void *var, int32_t v) {
    if (!anim_running) return;
    lv_ui *ui = var;
    if (!ui || !ui->screen_carDashboard_meter_cSpeed) return;
    
    if (ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0)
        lv_meter_set_indicator_value(ui->screen_carDashboard_meter_cSpeed,
            ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0, v);
    
    if (ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0)
        lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_cSpeed,
            ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0, clamp_arc(v));
    
    if (ui->screen_carDashboard_label_speedNum4) {
        char buf[8];
        int32_t val = (v-8)*2;
        sprintf(buf, "%d", val>240?240:(val<0?0:val));
        lv_label_set_text(ui->screen_carDashboard_label_speedNum4, buf);
        lv_label_set_text(ui->screen_carDashboard_label_largeNum, buf);
    }
}

/**********************
 * 启动/停止动画
 **********************/
void start_dashboard_animation(lv_ui *ui) {
    if (anim_running) return;
    anim_running = true;
    
    /* 初始化：隐藏所有指示灯 */
    ind_map_t all_maps[] = {
        IND_MAP(turnL, 0), IND_MAP(turnR, 0), IND_MAP(FrontFog, 0),
        IND_MAP(RearFog, 0), IND_MAP(HighBeam, 0), IND_MAP(ACC, 0),
        IND_MAP(Seatbelt, 0), IND_MAP(AbsOn, 0), IND_MAP(AbsOff, 0)
    };
    for (size_t i = 0; i < sizeof(all_maps)/sizeof(all_maps[0]); i++) {
        if (*all_maps[i].obj) lv_obj_add_flag(*all_maps[i].obj, LV_OBJ_FLAG_HIDDEN);
    }
    /* 激活指示灯（按需调整） */
    set_ind(IND_TURN_L, true);    // 左转向灯闪烁
    set_ind(IND_FRONT_FOG, true); // 前雾灯闪烁
    set_ind(IND_SEATBELT, true);  // 安全带常亮
    set_ind(IND_ACC, true);       // ACC常亮
    set_ind(IND_ABS, false);      // ABS常灭（故障时设为true）
    
    /* 启动闪烁定时器（防重复创建） */
    if (blink_timer) {
        lv_timer_del(blink_timer);
        blink_timer = NULL;
    }
    blink_timer = lv_timer_create(blink_cb, 500, ui);
    
    /* 转速表动画 */
    lv_anim_init(&rpm_anim);
    lv_anim_set_var(&rpm_anim, ui);
    lv_anim_set_exec_cb(&rpm_anim, set_rpm);
    lv_anim_set_values(&rpm_anim, 8, 116);
    lv_anim_set_time(&rpm_anim, 1800);
    lv_anim_set_playback_time(&rpm_anim, 1800);
    lv_anim_set_playback_delay(&rpm_anim, 5);
    lv_anim_set_repeat_count(&rpm_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&rpm_anim, lv_anim_path_ease_in_out);
    lv_anim_start(&rpm_anim);
    
    /* 车速表动画 */
    lv_anim_init(&spd_anim);
    lv_anim_set_var(&spd_anim, ui);
    lv_anim_set_exec_cb(&spd_anim, set_spd);
    lv_anim_set_values(&spd_anim, 8, 108);
    lv_anim_set_time(&spd_anim, 2200);
    lv_anim_set_playback_time(&spd_anim, 2200);
    lv_anim_set_playback_delay(&spd_anim, 10);
    lv_anim_set_repeat_count(&spd_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&spd_anim, lv_anim_path_ease_in_out);
    lv_anim_start(&spd_anim);
}

void stop_dashboard_animation(void) {
    if (!anim_running) return;
    anim_running = false;
    
    lv_anim_del(&rpm_anim, NULL);
    lv_anim_del(&spd_anim, NULL);
    
    if (blink_timer) {
        lv_timer_del(blink_timer);
        blink_timer = NULL;
    }
    ind_state = 0;
}