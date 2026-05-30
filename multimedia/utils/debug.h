#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define LOGE(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOGD(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define LOGE(fmt, ...)
#define LOGD(fmt, ...)
#endif

#endif