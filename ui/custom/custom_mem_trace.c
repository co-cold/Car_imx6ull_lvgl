/**
 * @file    custom_mem_trace.c
 * @brief   内存泄漏追踪实现
 */

#include "custom_mem_trace.h"

static mem_trace_record_t g_records[MEM_TRACE_MAX_RECORDS];
static int                g_record_count = 0;
static size_t             g_total_allocated = 0;
static size_t             g_total_freed = 0;

/* ========== 内部查找 ========== */

static int find_record(void *ptr) {
    for (int i = 0; i < g_record_count; i++) {
        if (g_records[i].ptr == ptr) return i;
    }
    return -1;
}

static int add_record(void *ptr, size_t size, const char *file, int line) {
    if (g_record_count >= MEM_TRACE_MAX_RECORDS) {
        fprintf(stderr, "[MEM_TRACE] WARN: record table full (%d entries), leak detection degraded\n",
                MEM_TRACE_MAX_RECORDS);
        return -1;
    }
    int idx = g_record_count++;
    g_records[idx].ptr  = ptr;
    g_records[idx].size = size;
    g_records[idx].file = file;
    g_records[idx].line = line;
    return 0;
}

static void remove_record(int idx) {
    g_records[idx] = g_records[g_record_count - 1];
    g_record_count--;
}

/* ========== 对外接口 ========== */

void mem_trace_init(void) {
    memset(g_records, 0, sizeof(g_records));
    g_record_count = 0;
    g_total_allocated = 0;
    g_total_freed = 0;
    printf("[MEM_TRACE] initialized, max records=%d\n", MEM_TRACE_MAX_RECORDS);
}

void *_mem_trace_malloc(size_t size, const char *file, int line) {
    void *p = malloc(size);
    if (p) {
        add_record(p, size, file, line);
        g_total_allocated += size;
    }
    return p;
}

void *_mem_trace_calloc(size_t nmemb, size_t size, const char *file, int line) {
    void *p = calloc(nmemb, size);
    if (p) {
        add_record(p, nmemb * size, file, line);
        g_total_allocated += nmemb * size;
    }
    return p;
}

void _mem_trace_free(void *ptr) {
    if (!ptr) return;

    int idx = find_record(ptr);
    if (idx >= 0) {
        g_total_freed += g_records[idx].size;
        remove_record(idx);
    }
    free(ptr);
}

void mem_trace_stats(void) {
    size_t leak_bytes = g_total_allocated - g_total_freed;
    printf("[MEM_TRACE] stats: allocated=%zu freed=%zu leaked=%zu active_records=%d\n",
           g_total_allocated, g_total_freed, leak_bytes, g_record_count);
}

void mem_trace_dump(void) {
    if (g_record_count == 0) {
        printf("[MEM_TRACE] no leaks detected\n");
        return;
    }

    size_t total_leak = 0;
    printf("\n========== MEMORY LEAK REPORT: %d record(s) ==========\n", g_record_count);
    for (int i = 0; i < g_record_count; i++) {
        mem_trace_record_t *r = &g_records[i];
        printf("  [%d] %p  size=%-6zu  %s:%d\n",
               i, r->ptr, r->size, r->file, r->line);
        total_leak += r->size;
    }
    printf("========== TOTAL LEAKED: %zu bytes (%zu KB) ==========\n\n",
           total_leak, total_leak / 1024);
}