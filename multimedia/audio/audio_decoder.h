#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

/*
 * audio_decoder.h — 音频解码器接口
 *
 * 使用 libavcodec/FFmpeg 将压缩音频（MP3/AAC）解码为 PCM 数据，
 * 送入环形缓冲区供 audio_output 消费。
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "ring_buffer.h"



/**
 * @brief 音频解码器上下文结构体
 */
typedef struct {
    char *file_path;                        /**< 输入文件路径 */
    struct AVFormatContext *fmt_ctx;        /**< FFmpeg 格式上下文 */
    struct AVCodecContext *codec_ctx;       /**< FFmpeg 编解码器上下文 */
    struct SwrContext *swr_ctx;             /**< 重采样上下文 */
    int audio_stream_idx;                   /**< 音频流索引 */
    RingBuffer *rb;                         /**< 输出环形缓冲区 */
    pthread_t thread;                       /**< 解码线程 ID */
    volatile int running;                   /**< 线程运行标志 */
    volatile int seek_req;                  /**< 跳转请求时间（毫秒） */
    double duration;                        /**< 媒体总时长（秒） */
    double current_time;                    /**< 当前播放时间（秒） */
    int sample_rate;                        /**< 音频文件原生采样率 */
    int channels;                           /**< 音频文件声道数 */
    
    // 内部同步变量，用于线程退出确认
    pthread_mutex_t exit_lock;              /**< 退出同步锁 */
    pthread_cond_t exit_cond;               /**< 退出同步条件变量 */
    volatile int thread_exited;             /**< 线程退出标志 */
} AudioDecoder;

/**
 * @brief 初始化音频解码器
 * @param file_path 音频文件路径
 * @param rb 环形缓冲区指针，用于输出 PCM 数据
 * @return 成功返回 AudioDecoder 指针，失败返回 NULL
 */
AudioDecoder* audio_decoder_init(const char *file_path, RingBuffer *rb);

/**
 * @brief 启动音频解码线程
 * @param ad 音频解码器实例
 */
void audio_decoder_start(AudioDecoder *ad);

/**
 * @brief 停止并等待音频解码线程退出
 * @param ad 音频解码器实例
 */
void audio_decoder_stop(AudioDecoder *ad);

/**
 * @brief 请求跳转到指定时间点
 * @param ad 音频解码器实例
 * @param seconds 目标时间（单位：秒）
 */
void audio_decoder_seek(AudioDecoder *ad, double seconds);

/**
 * @brief 获取当前音频播放位置
 * @param ad 音频解码器实例
 * @return 当前播放时间（秒）
 */
double audio_decoder_get_position(AudioDecoder *ad);

/**
 * @brief 释放音频解码器所有资源
 * @param ad 音频解码器实例
 */
void audio_decoder_free(AudioDecoder *ad);
#endif