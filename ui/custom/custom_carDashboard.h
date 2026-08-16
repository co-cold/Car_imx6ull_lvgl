#ifndef __CUSTOM_CARDASHBOARD_H__
#define __CUSTOM_CARDASHBOARD_H__

#include "gui_guider.h"
#include "lvgl.h"

/**
 * @brief 初始化仪表盘数据驱动
 *        内部会初始化 CAN + 创建 LVGL 定时器
 * @param ui  GUI Guider 的 UI 结构体指针
 * @return 0 成功, -1 失败
 */
int car_dashboard_init(lv_ui *ui);

/**
 * @brief 反初始化，清理资源
 */
void car_dashboard_deinit(void);


#endif