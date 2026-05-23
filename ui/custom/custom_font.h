#ifndef CUSTOM_FONT_H
#define CUSTOM_FONT_H

#include "lvgl.h"
#include <stdio.h>

// 调试宏控制
#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#define LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define LOGD(fmt, ...)
#define LOGE(fmt, ...)
#endif

const lv_font_t* custom_font_init(const char *font_path, uint32_t font_size);
const lv_font_t* custom_font_get(void);
const lv_font_t* custom_font_get_lyric(void);  // 歌词专用字体（比普通字体大2号）

#endif