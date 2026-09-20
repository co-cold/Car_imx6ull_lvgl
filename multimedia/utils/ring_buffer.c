/*
 * ring_buffer.c — 环形缓冲区实现
 *
 * 生产者/消费者模式，支持阻塞等待和超时。
 */
#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "debug.h"

/* 内部辅助：把 ptr 唤醒所有等待线程的便捷函数 */
static void wakeup_all(RingBuffer *rb) {
    pthread_cond_broadcast(&rb->not_full);
    pthread_cond_broadcast(&rb->not_empty);
}

RingBuffer* rb_init(int bytes) {
    RingBuffer *rb = (RingBuffer*)calloc(1, sizeof(RingBuffer));
    if (!rb) return NULL;

    rb->buf = (uint8_t*)malloc(bytes);
    if (!rb->buf) {
        free(rb);
        return NULL;
    }

    rb->capacity = bytes;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->abort_flag = 0;

    if (pthread_mutex_init(&rb->lock, NULL) != 0) {
        free(rb->buf);
        free(rb);
        return NULL;
    }
    if (pthread_cond_init(&rb->not_full, NULL) != 0) {
        pthread_mutex_destroy(&rb->lock);
        free(rb->buf);
        free(rb);
        return NULL;
    }
    if (pthread_cond_init(&rb->not_empty, NULL) != 0) {
        pthread_cond_destroy(&rb->not_full);
        pthread_mutex_destroy(&rb->lock);
        free(rb->buf);
        free(rb);
        return NULL;
    }

    return rb;
}

void rb_free(RingBuffer *rb) {
    LOGD("rb_free: 开始释放环形缓冲区\n");
    if (!rb) {
        LOGD("rb_free: rb为NULL，直接返回\n");
        return;
    }
    
    // 标记正在销毁，防止其他线程继续操作
    LOGD("rb_free: 设置destroy_in_progress标志\n");
    rb->destroy_in_progress = 1;
    
    LOGD("rb_free: 尝试获取锁\n");
    // 获取锁并设置终止标志，一次性完成所有资源销毁
    int ret = pthread_mutex_lock(&rb->lock);
    if (ret != 0) {
        LOGD("rb_free: 获取锁失败，错误码=%d\n", ret);
        // 即使获取锁失败，也要尝试释放内存
        free(rb->buf);
        free(rb);
        return;
    }
    LOGD("rb_free: 成功获取锁\n");
    
    rb->abort_flag = 1;
    // 唤醒所有等待的线程，使它们能尽快检测到终止标志并退出
    LOGD("rb_free: 唤醒所有等待的线程\n");
    pthread_cond_broadcast(&rb->not_full);
    pthread_cond_broadcast(&rb->not_empty);
    
    // 销毁资源（在同一个锁内完成）
    LOGD("rb_free: 销毁条件变量\n");
    ret = pthread_cond_destroy(&rb->not_full);
    if (ret != 0) {
        LOGD("rb_free: 销毁not_full条件变量失败，错误码=%d\n", ret);
    }
    ret = pthread_cond_destroy(&rb->not_empty);
    if (ret != 0) {
        LOGD("rb_free: 销毁not_empty条件变量失败，错误码=%d\n", ret);
    }
    LOGD("rb_free: 条件变量已销毁\n");
    
    pthread_mutex_unlock(&rb->lock);
    LOGD("rb_free: 解锁\n");
    
    LOGD("rb_free: 销毁互斥锁\n");
    ret = pthread_mutex_destroy(&rb->lock);
    if (ret != 0) {
        LOGD("rb_free: 销毁互斥锁失败，错误码=%d\n", ret);
    }
    LOGD("rb_free: 互斥锁已销毁\n");

    free(rb->buf);
    free(rb);
    LOGD("rb_free: 完成环形缓冲区释放\n");
}

int rb_write(RingBuffer *rb, const uint8_t *data, int len) {
    if (!rb || !data || len <= 0) return -1;

    pthread_mutex_lock(&rb->lock);

    // 检查是否正在销毁
    if (rb->destroy_in_progress) {
        pthread_mutex_unlock(&rb->lock);
        return -1;
    }

    int written = 0;
    while (written < len && !rb->abort_flag && !rb->destroy_in_progress) {
        // 如果空间不够则等待
        while (rb->count == rb->capacity && !rb->abort_flag && !rb->destroy_in_progress) {
            pthread_cond_wait(&rb->not_full, &rb->lock);
        }
        if (rb->abort_flag || rb->destroy_in_progress) break;

        int space = rb->capacity - rb->count;
        int to_write = len - written;
        if (to_write > space) to_write = space;

        int this_round = 0;
        // 可能从头环绕写入
        int first_part = rb->capacity - rb->head;
        if (to_write > first_part) {
            memcpy(rb->buf + rb->head, data + written, first_part);
            rb->head = 0;
            written += first_part;
            this_round += first_part;
            to_write -= first_part;
        }
        // 写剩余部分
        memcpy(rb->buf + rb->head, data + written, to_write);
        rb->head += to_write;
        if (rb->head == rb->capacity) rb->head = 0;
        written += to_write;
        this_round += to_write;

        rb->count += this_round;  // 正确累加本轮写入的字节数
    }

    // 写完后通知读者
    pthread_cond_broadcast(&rb->not_empty);
    pthread_mutex_unlock(&rb->lock);

    return (rb->abort_flag || rb->destroy_in_progress ? -1 : written);
}

int rb_read(RingBuffer *rb, uint8_t *out, int max_len) {
    if (!rb || !out || max_len <= 0) return -1;

    pthread_mutex_lock(&rb->lock);

    // 检查是否正在销毁
    if (rb->destroy_in_progress) {
        pthread_mutex_unlock(&rb->lock);
        return -1;
    }

    // 如果缓冲区为空且未中止，等待数据到来
    while (rb->count == 0 && !rb->abort_flag && !rb->destroy_in_progress) {
        pthread_cond_wait(&rb->not_empty, &rb->lock);
    }
    if (rb->abort_flag || rb->destroy_in_progress) {
        pthread_mutex_unlock(&rb->lock);
        return -1;
    }

    int to_read = (max_len < rb->count) ? max_len : rb->count;
    int read_bytes = 0;
    int remaining = to_read;

    int first_part = rb->capacity - rb->tail;
    if (remaining > first_part) {
        // 先读尾部到末尾
        memcpy(out, rb->buf + rb->tail, first_part);
        read_bytes += first_part;
        rb->tail = 0;
        remaining -= first_part;
        rb->count -= first_part;
    }
    // 读剩余部分
    memcpy(out + read_bytes, rb->buf + rb->tail, remaining);
    rb->tail += remaining;
    if (rb->tail == rb->capacity) rb->tail = 0;
    read_bytes += remaining;
    rb->count -= remaining;

    // 读完通知写者
    pthread_cond_broadcast(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);

    return read_bytes;
}

void rb_reset(RingBuffer *rb) {
    if (!rb) return;
    pthread_mutex_lock(&rb->lock);
    if (!rb->abort_flag && !rb->destroy_in_progress) {  // 确保缓冲区还没有被标记为销毁
        rb->head = 0;
        rb->tail = 0;
        rb->count = 0;
        // 唤醒可能在等待满条件的写者，但重置后空间全空闲
        pthread_cond_broadcast(&rb->not_full);
    }
    pthread_mutex_unlock(&rb->lock);
}

/**
 * @brief 唤醒所有等待的线程（用于安全停止）
 */
void rb_wakeup_all(RingBuffer *rb) {
    if (!rb) return;
    pthread_mutex_lock(&rb->lock);
    if (!rb->abort_flag && !rb->destroy_in_progress) {  // 确保缓冲区还没有被标记为销毁
        pthread_cond_broadcast(&rb->not_full);
        pthread_cond_broadcast(&rb->not_empty);
    }
    pthread_mutex_unlock(&rb->lock);
}