#include "custom_carDashboard.h"
#include <stdio.h>
#include <stdlib.h>

static lv_ui *g_ui = NULL;

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

static void blink_cb(lv_timer_t *t) {
    lv_ui *ui = g_ui;
    if (!ui) return;
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

    /* ABS：故障时常亮 */
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

/* 转速表动画回调 */
static void set_rpm(void *var, int32_t v) {
    if (!anim_running) return;
    lv_ui *ui = g_ui;
    if (!ui || !ui->screen_carDashboard_meter_rSpeed) return;

    if (ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0)
        lv_meter_set_indicator_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0, v);

    if (ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0)
        lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0, clamp_arc(v));

    if (ui->screen_carDashboard_label_speedNum1) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", (v-8)*100);
        lv_label_set_text(ui->screen_carDashboard_label_speedNum1, buf);
    }
}

/* 车速表动画回调 */
static void set_spd(void *var, int32_t v) {
    if (!anim_running) return;
    lv_ui *ui = g_ui;
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
        val = val > 240 ? 240 : (val < 0 ? 0 : val);
        snprintf(buf, sizeof(buf), "%d", val);
        lv_label_set_text(ui->screen_carDashboard_label_speedNum4, buf);
        if (ui->screen_carDashboard_label_largeNum)
            lv_label_set_text(ui->screen_carDashboard_label_largeNum, buf);
    }
}

static void start_animation(void) {
    lv_ui *ui = g_ui;
    if (!ui) return;
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

    /* 激活 Demo 指示灯 */
    set_ind(IND_TURN_L, true);
    set_ind(IND_FRONT_FOG, true);
    set_ind(IND_SEATBELT, true);
    set_ind(IND_ACC, true);
    set_ind(IND_ABS, false);

    /* 闪烁定时器 */
    if (!blink_timer) blink_timer = lv_timer_create(blink_cb, 500, ui);

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

static void stop_animation(void) {
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

/* ──── meter arc 图片注入 ──── */
static void meter_arc_img_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
        if (dsc->type == LV_METER_DRAW_PART_ARC && dsc->arc_dsc) {
            dsc->arc_dsc->img_src = &_gauge_indicator_alpha_220x220;
        }
    }
}

void setup_meter_arc_image(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_carDashboard_meter_cSpeed, meter_arc_img_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(ui->screen_carDashboard_meter_rSpeed, meter_arc_img_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

/* ========== 公开 API ========== */
int car_dashboard_init(lv_ui *ui)
{
    if (!ui) return -1;
    g_ui = ui;
    start_animation();
    return 0;
}

void car_dashboard_deinit(void)
{
    stop_animation();
    g_ui = NULL;
}