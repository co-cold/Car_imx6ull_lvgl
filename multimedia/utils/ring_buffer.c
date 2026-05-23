#include "ring_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

    pthread_mutex_init(&rb->lock, NULL);
    pthread_cond_init(&rb->not_full, NULL);
    pthread_cond_init(&rb->not_empty, NULL);

    return rb;
}

void rb_free(RingBuffer *rb) {
    if (!rb) return;
    pthread_mutex_lock(&rb->lock);
    rb->abort_flag = 1;
    wakeup_all(rb);                     // 唤醒所有等待的线程
    pthread_mutex_unlock(&rb->lock);

    pthread_cond_destroy(&rb->not_full);
    pthread_cond_destroy(&rb->not_empty);
    pthread_mutex_destroy(&rb->lock);

    free(rb->buf);
    free(rb);
}

int rb_write(RingBuffer *rb, const uint8_t *data, int len) {
    if (!rb || !data || len <= 0) return -1;

    pthread_mutex_lock(&rb->lock);

    int written = 0;
    while (written < len && !rb->abort_flag) {
        // 如果空间不够则等待
        while (rb->count == rb->capacity && !rb->abort_flag) {
            pthread_cond_wait(&rb->not_full, &rb->lock);
        }
        if (rb->abort_flag) break;

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

    return (rb->abort_flag ? -1 : written);
}

int rb_read(RingBuffer *rb, uint8_t *out, int max_len) {
    if (!rb || !out || max_len <= 0) return -1;

    pthread_mutex_lock(&rb->lock);

    // 如果缓冲区为空且未中止，等待数据到来
    while (rb->count == 0 && !rb->abort_flag) {
        pthread_cond_wait(&rb->not_empty, &rb->lock);
    }
    if (rb->abort_flag) {
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
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    // 唤醒可能在等待满条件的写者，但重置后空间全空闲
    pthread_cond_broadcast(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);
}

/**
 * @brief 唤醒所有等待的线程（用于安全停止）
 */
void rb_wakeup_all(RingBuffer *rb) {
    if (!rb) return;
    pthread_mutex_lock(&rb->lock);
    wakeup_all(rb);
    pthread_mutex_unlock(&rb->lock);
}