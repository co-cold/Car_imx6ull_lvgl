#ifndef _REVERSE_TRAINING_H_
#define _REVERSE_TRAINING_H_

/*
 * 倒车入库训练状态机
 *
 * 功能：管理倒车入库训练的完整流程。
 *
 * 状态转移：
 *   IDLE → ACTIVE（检测到倒车）→ JUDGING（停车）→ SCORING（打分）→ FINISHED
 *
 * 6级距离分段：
 *   0 绿色      >100cm     安全
 *   1 黄色      50-100cm   请减速
 *   2 橙色      30-50cm    请准备停车
 *   3 绿色高亮   20-30cm    停车到位 ✓  （目标区间）
 *   4 红色      10-20cm    距离过近！
 *   5 红色闪烁   <10cm      立即停车！！
 *
 * 评分算法（满分100）：
 *   急刹 1次     -20
 *   急加速 1次   -15
 *   转向过猛 1次 -10
 *   停车不平稳   -15（停车瞬间加速度 > 0.3g）
 *   最终距离不在20-30cm  -20
 */

#include <stdint.h>
#include <stdbool.h>

// 训练状态
typedef enum {
    TRAINING_IDLE = 0,       // 空闲，等待开始
    TRAINING_ACTIVE,         // 倒车中
    TRAINING_JUDGING,        // 停车判定中
    TRAINING_SCORING,        // 评分计算中
    TRAINING_FINISHED        // 训练完成
} training_state_t;

// 距离分级
typedef enum {
    DIST_GREEN     = 0,      // >100cm  安全
    DIST_YELLOW    = 1,      // 50-100cm 请减速
    DIST_ORANGE    = 2,      // 30-50cm  准备停车
    DIST_GREEN_OK  = 3,      // 20-30cm  停车到位 ✓
    DIST_RED       = 4,      // 10-20cm  过近
    DIST_RED_FLASH = 5       // <10cm   立即停车
} distance_grade_t;

// 训练结果
typedef struct {
    training_state_t state;              // 当前状态
    distance_grade_t grade;              // 当前距离分级

    /* —— 最终结果 —— */
    float    final_distance_cm;          // 停车时时锁定距离
    bool     passed;                     // 是否合格（20-30cm）

    /* —— 评分 —— */
    int      score;                      // 综合评分（0-100）
    int      brake_penalties;            // 急刹扣分
    int      accel_penalties;            // 急加速扣分
    int      turn_penalties;             // 转向过猛扣分
    int      harsh_stop_penalty;         // 停车不平稳扣分

    /* —— 统计 —— */
    float    max_accel;                  // 最大加速度
    float    max_speed_cms;             // 最大速度
    float    avg_speed_cms;             // 平均速度
    int      speed_samples;              // 速度样本数

    /* —— UI 提示 —— */
    const char *hint_text;               // 当前提示文字
} Training_State_t;

// ========== 训练接口 ==========
int  reverse_training_init(void);                            // 初始化
void reverse_training_deinit(void);                          // 反初始化

void reverse_training_start(void);                           // 开始训练
void reverse_training_stop(void);                            // 停止训练

void reverse_training_update(void);                          // 状态机更新（~100ms调用）

int  reverse_training_get_state(Training_State_t *out);     // 获取当前状态

#endif