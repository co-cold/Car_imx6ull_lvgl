#include "custom_carDashboard.h"
#include "ipc/ipc_can.h"
#include <stdio.h> 

static lv_ui *g_ui = NULL;         // 保存 UI 指针供定时器用
static lv_timer_t *dash_timer = NULL;

/* 限幅 */
static int32_t clamp(int32_t v, int32_t min, int32_t max)
{
    return v < min ? min : (v > max ? max : v);
}

/* LVGL meter arc 限幅 (8~108) */
static int32_t clamp_arc(int32_t v)
{
    return clamp(v, 8, 108);
}

/**
 * 定时器回调：每 100ms 从 CAN 驱动获取编码器数据并更新仪表盘显示
 */
static void dash_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    Encoder_Data_t enc;

    /* 从 IPC 层读编码器数据，只需消费 */
    if (ipc_can_get_encoder(&enc) != 0)
        return;
    /* ──────── 2. 数据 → 仪表盘映射 ──────── */
    lv_ui *ui = g_ui;

    /* ── 转速表 (右表) ──
     *    meter 范围: 8 ~ 116
     *    enc.count = OBD-II 实际转速 (RPM), 范围 0 ~ 8000
     *    映射: rpm_val = 8 + (int)count * 108 / 8000
     */
    int32_t rpm_val = 8 + ((int32_t)enc.count * 108 / 8000);
    rpm_val = clamp_arc(rpm_val);

    if (ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0)
        lv_meter_set_indicator_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_ndline_0, rpm_val);

    if (ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0)
        lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_rSpeed,
            ui->screen_carDashboard_meter_rSpeed_scale_0_arc_0, clamp_arc(rpm_val));

    /* 转速数字 */
    if (ui->screen_carDashboard_label_speedNum1) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", enc.count);
        lv_label_set_text(ui->screen_carDashboard_label_speedNum1, buf);
    }

    /* ── 车速表 (左表) ──
     *    meter 范围: 8 ~ 108
     *    enc.speed_rpm = OBD-II 实际车速 (km/h), 范围 0 ~ 240
     *    映射: spd_val = 8 + (int)(speed * 100 / 240)
     */
    int32_t spd_val = 8 + (int32_t)(enc.speed_rpm * 100.0f / 240.0f);
    spd_val = clamp_arc(spd_val);

    if (ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0)
        lv_meter_set_indicator_value(ui->screen_carDashboard_meter_cSpeed,
            ui->screen_carDashboard_meter_cSpeed_scale_0_ndline_0, spd_val);

    if (ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0)
        lv_meter_set_indicator_end_value(ui->screen_carDashboard_meter_cSpeed,
            ui->screen_carDashboard_meter_cSpeed_scale_0_arc_0, clamp_arc(spd_val));

    /* 车速数字 */
    if (ui->screen_carDashboard_label_speedNum4) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", (int)enc.speed_rpm);
        lv_label_set_text(ui->screen_carDashboard_label_speedNum4, buf);
    }
}

/* ========== 公开 API ========== */
int car_dashboard_init(lv_ui *ui)
{
    if (!ui) return -1;

    g_ui  = ui;

    dash_timer = lv_timer_create(dash_timer_cb, 50, NULL);
    return 0;
}

void car_dashboard_deinit(void)
{
    if (dash_timer) {
        lv_timer_del(dash_timer);
        dash_timer = NULL;
    }
    g_ui = NULL;
}