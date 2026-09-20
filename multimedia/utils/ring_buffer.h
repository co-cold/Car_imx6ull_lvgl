#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/*
 * ring_buffer.h — 线程安全环形缓冲区
 *
 * 用于音频/视频数据在生产者（解码器）和消费者（输出/渲染）之间传递。
 * 支持阻塞和非阻塞读写。
 */

#include <stdint.h>
#include <pthread.h>

/**
 * @brief 线程安全的环形缓冲区结构体
 */
typedef struct {
    uint8_t   *buf;             /**< 实际存储区域 */
    int        capacity;        /**< 总字节数 */
    int        head;            /**< 写入位置（索引） */
    int        tail;            /**< 读取位置（索引） */
    int        count;           /**< 当前有效数据字节数 */
    pthread_mutex_t lock;       /**< 互斥锁 */
    pthread_cond_t  not_full;   /**< 条件变量：非满（可写） */
    pthread_cond_t  not_empty;  /**< 条件变量：非空（可读） */
    volatile int abort_flag;     /**< 用于安全退出：1表示缓冲区已销毁，读写立即返回错误 */
    volatile int destroy_in_progress;  /**< 标记是否正在销毁 */
} RingBuffer;

/**
 * @brief 创建并初始化环形缓冲区
 * @param bytes 缓冲区总容量（字节）
 * @return 成功返回RingBuffer指针，失败返回NULL
 */
RingBuffer* rb_init(int bytes);

/**
 * @brief 销毁环形缓冲区，唤醒所有等待线程
 */
void rb_free(RingBuffer *rb);

/**
 * @brief 写入数据到缓冲区（阻塞直到写入全部len字节）
 * @param data 源数据指针
 * @param len  要写入的字节数
 * @return 实际写入的字节数（通常等于len，出错返回-1）
 */
int rb_write(RingBuffer *rb, const uint8_t *data, int len);

/**
 * @brief 从缓冲区读取数据（阻塞直到读取到至少1字节，返回实际读取数）
 * @param out     输出缓冲区
 * @param max_len 期望读取的最大字节数
 * @return 实际读取的字节数（>0），失败或缓冲区被销毁返回-1
 */
int rb_read(RingBuffer *rb, uint8_t *out, int max_len);

/**
 * @brief 清空缓冲区内容（通常在Seek时使用）
 */
void rb_reset(RingBuffer *rb);

/**
 * @brief 唤醒所有等待的线程（用于安全停止）
 */
void rb_wakeup_all(RingBuffer *rb);

#endif