/**
 * @file    custom_mem_trace.h
 * @brief   内存泄漏追踪工具：拦截 malloc/calloc/free，记录分配栈
 *
 * 使用方法：
 *   1. 在 custom_mem_trace.h 被 include 之前定义 CUSTOM_MEM_TRACE_ENABLE=1
 *   2. 程序退出前调用 mem_trace_dump() 打印所有未释放的分配
 *   3. 运行时调用 mem_trace_stats() 查看当前内存用量
 *
 * 原理：
 *   用宏替换 malloc/calloc/free，每次分配记录文件名、行号、大小。
 *   释放时从追踪表中移除。程序结束时，表中剩余的就是泄漏。
 */

#ifndef CUSTOM_MEM_TRACE_H
#define CUSTOM_MEM_TRACE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 配置 ========== */
#define MEM_TRACE_MAX_RECORDS 256

/* ========== 数据结构 ========== */
typedef struct {
    void       *ptr;
    size_t      size;
    const char *file;
    int         line;
} mem_trace_record_t;

/* ========== 初始化/开关 ========== */
void mem_trace_init(void);
void mem_trace_dump(void);
void mem_trace_stats(void);

/* ========== 内部 API（由宏调用） ========== */
void *_mem_trace_malloc(size_t size, const char *file, int line);
void *_mem_trace_calloc(size_t nmemb, size_t size, const char *file, int line);
void  _mem_trace_free(void *ptr);

#ifdef CUSTOM_MEM_TRACE_ENABLE

    /* 替换标准 malloc/calloc/free */
    #define malloc(s)      _mem_trace_malloc((s), __FILE__, __LINE__)
    #define calloc(n, s)   _mem_trace_calloc((n), (s), __FILE__, __LINE__)
    #define free(p)        _mem_trace_free(p)

#endif /* CUSTOM_MEM_TRACE_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_MEM_TRACE_H */