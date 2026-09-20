/*
 * reverse_training.c — 倒车入库训练状态机实现
 *
 * 状态转移：
 *   IDLE → ACTIVE（倒车+接近）→ JUDGING（停车）→ SCORING（打分）→ FINISHED
 *
 * 距离分级：6级颜色提示（绿→黄→橙→绿高亮→红→红闪）
 * 评分算法：满分100，急刹-20/急加速-15/转向-10/停车不稳-15/距离不对-20
 *
 * 数据来源：sensor_fusion_get_data() — UWB距离 + OBD车速 + MPU6050姿态
 */
#include "reverse_training.h"
#include "sensor_fusion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ========== 训练内部状态 ==========

static struct {
    Training_State_t state;
    bool             active;              // 训练是否激活

    /* 速度统计 */
    float            speed_sum;
    int              speed_cnt;
    float            max_speed;
    float            max_accel;

    /* 停车判定 */
    float            locked_distance_cm;
    bool             distance_locked;
    bool             scored;
} g_train;

static distance_grade_t classify_distance(float dist_cm)
{
    if (dist_cm > 100.0f) return DIST_GREEN;
    if (dist_cm > 50.0f)  return DIST_YELLOW;
    if (dist_cm > 30.0f)  return DIST_ORANGE;
    if (dist_cm >= 20.0f) return DIST_GREEN_OK;
    if (dist_cm >= 10.0f) return DIST_RED;
    return DIST_RED_FLASH;
}

static const char *grade_hint(distance_grade_t g)
{
    switch (g) {
        case DIST_GREEN:     return "";
        case DIST_YELLOW:    return "请减速";
        case DIST_ORANGE:    return "请准备停车";
        case DIST_GREEN_OK:  return "停车到位!";
        case DIST_RED:       return "距离过近!";
        case DIST_RED_FLASH: return "立即停车!";
        default:             return "";
    }
}

// ========== 公共 API ==========

int reverse_training_init(void)
{
    memset(&g_train, 0, sizeof(g_train));
    g_train.state.state = TRAINING_IDLE;
    printf("[training] initialized\n");
    return 0;
}

void reverse_training_deinit(void)
{
    g_train.active = false;
    printf("[training] deinitialized\n");
}

void reverse_training_start(void)
{
    memset(&g_train, 0, sizeof(g_train));
    g_train.state.state = TRAINING_IDLE;
    g_train.active = true;

    /* 重置融合统计 */
    sensor_fusion_reset_stats();

    printf("[training] started\n");
}

void reverse_training_stop(void)
{
    g_train.active = false;
    g_train.state.state = TRAINING_IDLE;
    printf("[training] stopped\n");
}

void reverse_training_update(void)
{
    if (!g_train.active) return;

    Fusion_Data_t fdata;
    if (sensor_fusion_get_data(&fdata) != 0) return;

    float dist_cm = fdata.distance_cm;
    float speed   = fdata.speed_fused_cms;
    distance_grade_t grade = classify_distance(dist_cm);

    g_train.state.grade       = grade;
    g_train.state.hint_text   = grade_hint(grade);

    /* 速度统计 */
    if (fdata.uwb_valid) {
        g_train.speed_sum += fabsf(speed);
        g_train.speed_cnt++;
        if (fabsf(speed) > g_train.max_speed)
            g_train.max_speed = fabsf(speed);
        g_train.state.max_speed_cms = g_train.max_speed;

        if (g_train.speed_cnt > 0)
            g_train.state.avg_speed_cms = g_train.speed_sum / (float)g_train.speed_cnt;
        g_train.state.speed_samples = g_train.speed_cnt;
    }

    /* 加速度峰值 */
    if (fdata.accel_peak > g_train.max_accel)
        g_train.max_accel = fdata.accel_peak;
    g_train.state.max_accel = g_train.max_accel;

    /* ── 状态机 ──────────────────────────────── */
    if (g_train.state.state == TRAINING_IDLE) {
        /* 检测到 UWB 有效距离 且 有速度 → 进入倒车监测 */
        if (fdata.uwb_valid && dist_cm > 1.0f && fabsf(speed) > 0.5f) {
            g_train.state.state = TRAINING_ACTIVE;
            printf("[training] entering ACTIVE, dist=%.1fcm speed=%.1fcm/s\n",
                   dist_cm, speed);
        }
    }

    if (g_train.state.state == TRAINING_ACTIVE) {
        /* 检测停车：速度接近 0 且距离稳定 */
        if (fabsf(speed) < 0.5f && dist_cm > 1.0f) {
            if (!g_train.distance_locked) {
                g_train.locked_distance_cm = dist_cm;
                g_train.distance_locked = true;
                g_train.state.state = TRAINING_JUDGING;
                printf("[training] JUDGING: locked distance=%.1fcm\n", dist_cm);
            }
        } else {
            g_train.distance_locked = false;
        }
    }

    if (g_train.state.state == TRAINING_JUDGING) {
        /* 如果又开始移动，回到监测 */
        if (fabsf(speed) > 0.5f) {
            g_train.distance_locked = false;
            g_train.state.state = TRAINING_ACTIVE;
        }
    }

    /* 更新最终距离（持续更新直到评分完成） */
    if (g_train.state.state == TRAINING_JUDGING ||
        g_train.state.state == TRAINING_SCORING) {
        g_train.state.final_distance_cm = g_train.locked_distance_cm;
    }
}

/* ── 获取评分结果 ──────────────────────────────────── */
int reverse_training_get_state(Training_State_t *out)
{
    if (!out || !g_train.active) return -1;

    Fusion_Data_t fdata;
    sensor_fusion_get_data(&fdata);

    Training_State_t s;
    memcpy(&s, &g_train.state, sizeof(s));

    /* 如果处于打分状态（或手动触发），计算评分 */
    if (s.state == TRAINING_JUDGING) {
        int score = 100;

        /* 急刹扣分 */
        s.brake_penalties  = fdata.brake_count;
        score -= fdata.brake_count * 20;

        /* 急加速扣分 */
        s.accel_penalties  = fdata.accel_count;
        score -= fdata.accel_count * 15;

        /* 转向过猛扣分 */
        s.turn_penalties   = fdata.turn_count;
        score -= fdata.turn_count * 10;

        /* 停车不平顺（加速度峰值 > 0.3g） */
        if (g_train.max_accel > 0.3f * 9.8f) {
            s.harsh_stop_penalty = 15;
            score -= 15;
        } else {
            s.harsh_stop_penalty = 0;
        }

        if (score < 0) score = 0;
        s.score = score;

        /* 判定是否合格 */
        float dist = s.final_distance_cm;
        s.passed = (dist >= 20.0f && dist <= 30.0f);
    }

    s.max_accel = g_train.max_accel;

    *out = s;
    return 0;
}