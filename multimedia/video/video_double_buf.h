#ifndef VIDEO_DOUBLE_BUF_H
#define VIDEO_DOUBLE_BUF_H

/*
 * video_double_buf.h — 视频帧双缓冲管理
 *
 * 两个缓冲区交替写入/读取，生产者（解码器）写入一帧，
 * 消费者（UI渲染）读取另一帧，避免撕裂。
 */

#include <stdint.h>
#include <pthread.h>

/**
 * @brief 视频双缓冲管理结构体
 */
typedef struct {
    uint8_t *buf[2];          /**< 两个缓冲区指针 */
    double pts[2];            /**< 每帧的 PTS（秒），-1 表示无效 */
    int buf_size;             /**< 每个缓冲区字节数 */
    int write_idx;            /**< 解码线程当前写入索引（0 或 1） */
    int ready_idx;            /**< 已完成且待 UI 读取的帧索引（-1 表示无） */
    pthread_mutex_t lock;     /**< 互斥锁 */
    pthread_cond_t cond;      /**< 条件变量，用于通知 UI 新帧就绪 */
    int ui_busy;              /**< 标志位：1 表示 UI 尚未取走上一帧，用于跳帧 */
    volatile int stopped;     /**< 停止标志：1 表示缓冲已被销毁 */
} VideoDoubleBuf;

/**
 * @param size 单个缓冲区的字节数（例如 width*height*2）
 */
VideoDoubleBuf* vdb_create(int size);
void vdb_destroy(VideoDoubleBuf *vdb);

// 解码线程使用：获取可写入的缓冲区指针
uint8_t* vdb_get_write_buf(VideoDoubleBuf *vdb);
// 解码线程使用：提交已写入完毕的帧（含 PTS）
void vdb_commit_with_pts(VideoDoubleBuf *vdb, double pts);
// 兼容旧接口
#define vdb_commit(vdb) vdb_commit_with_pts(vdb, -1.0)

// LVGL线程使用：获取已就绪的帧缓冲区（若没有则返回NULL）
uint8_t* vdb_get_ready_buf(VideoDoubleBuf *vdb);
// LVGL线程使用：根据音频时间获取最匹配的帧缓冲区
uint8_t* vdb_get_ready_buf_at_time(VideoDoubleBuf *vdb, double audio_time);
// LVGL线程使用：释放已读取的帧，允许写入
void vdb_release(VideoDoubleBuf *vdb);

#endif