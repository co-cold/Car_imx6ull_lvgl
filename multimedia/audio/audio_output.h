#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <alsa/asoundlib.h>
#include <stdint.h>
#include <stdio.h>
#include "ring_buffer.h"   // 来自 multimedia/utils

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

typedef struct {
    snd_pcm_t *handle;          // ALSA PCM句柄
    snd_mixer_t *mixer;         // ALSA Mixer句柄（硬件音量控制）
    snd_mixer_elem_t *elem;     // 音量控件元素
    long vol_min;               // 音量最小值
    long vol_max;               // 音量最大值
    
    unsigned int sample_rate;
    int channels;
    snd_pcm_format_t format;    // SND_PCM_FORMAT_S16_LE
    int period_size;            // 每次写入的帧数（period）

    RingBuffer *rb;             // 音频数据来源

    volatile int running;       // 控制线程：1=运行，0=停止
    volatile int paused;        // 1=暂停，0=播放中
    pthread_t thread;           // 播放线程ID
    
    float volume;               // 音量大小（0.0 - 2.0，1.0为原始音量）
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

/**
 * @brief 设置音量
 * @param ao      AudioOutput 实例
 * @param volume  音量值（0.0 - 2.0，1.0为原始音量）
 */
void audio_output_set_volume(AudioOutput *ao, float volume);

/**
 * @brief 获取当前音量
 * @param ao  AudioOutput 实例
 * @return 当前音量值
 */
float audio_output_get_volume(AudioOutput *ao);

/**
 * @brief 设置所有可用的 mixer 控件音量（用于 WM8960 等需要多个控件的芯片）
 * @param ao AudioOutput 实例
 * @param volume 音量值（0.0 - 2.0）
 */
void audio_output_set_volume_all(AudioOutput *ao, float volume);

#endif