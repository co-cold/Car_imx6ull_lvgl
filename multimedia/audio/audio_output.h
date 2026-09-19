#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <alsa/asoundlib.h>
#include <stdint.h>
#include <stdio.h>
#include "ring_buffer.h"

typedef struct {
    snd_pcm_t *handle;
    unsigned int sample_rate;
    int channels;
    snd_pcm_format_t format;
    int period_size;

    RingBuffer *rb;

    volatile int running;
    volatile int paused;
    pthread_t thread;

    pthread_mutex_t exit_lock;
    pthread_cond_t exit_cond;
    volatile int thread_exited;
} AudioOutput;

/**
 * @brief 初始化音频输出设备并创建环形缓冲区关联
 * @param sample_rate  采样率（如 44100）
 * @param channels     声道数（如 2）
 * @param period_size  期望的 period 大小（帧数），如 1024
 * @param rb           解码线程共享的环形缓冲区指针
 * @return 成功返回 AudioOutput 指针，失败返回 NULL
 */
AudioOutput* audio_output_init(unsigned int sample_rate, int channels,
                               int period_size, RingBuffer *rb);

/**
 * @brief 启动播放线程（内部创建 pthread）
 */
void audio_output_start(AudioOutput *ao);

/**
 * @brief 暂停播放（线程仍存活，但停止写入声卡）
 */
void audio_output_pause(AudioOutput *ao);

/**
 * @brief 恢复播放
 */
void audio_output_resume(AudioOutput *ao);

/**
 * @brief 停止播放线程，等待其退出
 */
void audio_output_stop(AudioOutput *ao);

/**
 * @brief 释放所有资源（必须先停止）
 */
void audio_output_free(AudioOutput *ao);

#endif