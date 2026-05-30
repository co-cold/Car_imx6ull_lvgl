#define _GNU_SOURCE
#include "audio_output.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "../utils/debug.h"
#include <libavutil/samplefmt.h>
#include "../utils/ring_buffer.h"

/* ---------- 内部播放线程 ---------- */
static void* ao_thread_fn(void *arg) {
    AudioOutput *ao = (AudioOutput*)arg;
    // 根据实际配置的格式计算每帧字节数
    int bytes_per_sample = (ao->format == SND_PCM_FORMAT_S32_LE) ? sizeof(int32_t) : sizeof(int16_t);
    int frame_bytes = ao->channels * bytes_per_sample;
    int buf_frames = ao->period_size;                       // 缓冲帧数
    int buf_bytes  = buf_frames * frame_bytes;
    uint8_t *buf = (uint8_t*)malloc(buf_bytes);
    if (!buf) {
        LOGE("音频输出: 内存分配失败\n");
        return NULL;
    }

    while (1) {
        /* ---------- 检查退出标志 ---------- */
        if (!ao->running) {
            break;
        }
        
        /* ---------- 暂停处理 ---------- */
        if (ao->paused) {
            usleep(10000);          // 10ms 睡眠，防止 CPU 空转
            continue;
        }

        /* ---------- 从环形缓冲区读取一帧数据 ---------- */
        RingBuffer *rb_copy = ao->rb;  // 获取环形缓冲区指针的本地副本
        if (!rb_copy) {
            // 环形缓冲区已被释放，退出线程
            break;
        }
        int ret = rb_read(rb_copy, buf, buf_bytes);
        if (ret < 0) {
            // 缓冲区被销毁或发生错误，退出线程
            break;
        }
        // ret 可能小于 buf_bytes （正常阻塞读取会等够）
        if (ret == 0) continue;

        /* ---------- 检查句柄有效性 ---------- */
        snd_pcm_t *handle_copy = ao->handle;  // 获取PCM句柄的本地副本
        if (!handle_copy) {
            // PCM句柄已被释放，退出线程
            break;
        }

        /* ---------- 写入 ALSA 声卡 ---------- */
        int frames_to_write = ret / frame_bytes;
        int written = snd_pcm_writei(handle_copy, buf, frames_to_write);
        if (written < 0) {
            // 发生 XRUN 或其它错误，尝试恢复
            LOGE("音频输出: XRUN 恢复, written = %d\n", written);
            snd_pcm_recover(handle_copy, written, 1);
            // 跳过此次迭代，重新读取数据
            continue;
        }
    }

    free(buf);
    // 线程退出前清空声卡缓冲区，但要确保句柄仍然有效
    snd_pcm_t *handle_copy = ao->handle;  // 再次获取句柄副本
    if (handle_copy) {
        snd_pcm_drain(handle_copy);
    }
    
    // 设置线程退出标志并发出通知
    pthread_mutex_lock(&ao->exit_lock);
    ao->thread_exited = 1;
    pthread_cond_broadcast(&ao->exit_cond);
    pthread_mutex_unlock(&ao->exit_lock);
    
    return NULL;
}

/* ---------- 尝试打开并设置 mixer（支持多种常见控件名称） ---------- */
static int audio_output_init_mixer(AudioOutput *ao) {
    const char *ctrl_names[] = {"Master", "PCM", "Headphone", "Speaker", "Playback", NULL};
    int ret;
    
    // 打开 mixer
    ret = snd_mixer_open(&ao->mixer, 0);
    if (ret < 0) {
        LOGE("音频输出: snd_mixer_open 失败: %s\n", snd_strerror(ret));
        return -1;
    }
    
    // 附加默认声卡
    ret = snd_mixer_attach(ao->mixer, "default");
    if (ret < 0) {
        LOGE("音频输出: snd_mixer_attach 失败: %s\n", snd_strerror(ret));
        snd_mixer_close(ao->mixer);
        ao->mixer = NULL;
        return -1;
    }
    
    // 注册元素类型
    snd_mixer_selem_register(ao->mixer, NULL, NULL);
    
    // 加载 mixer 元素
    ret = snd_mixer_load(ao->mixer);
    if (ret < 0) {
        LOGE("音频输出: snd_mixer_load 失败: %s\n", snd_strerror(ret));
        snd_mixer_close(ao->mixer);
        ao->mixer = NULL;
        return -1;
    }
    
    // 尝试查找音量控件（按优先级顺序）
    for (int i = 0; ctrl_names[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, ctrl_names[i]);
        ao->elem = snd_mixer_find_selem(ao->mixer, sid);
        
        if (ao->elem) {
            // 获取音量范围
            snd_mixer_selem_get_playback_volume_range(ao->elem, &ao->vol_min, &ao->vol_max);
            LOGD("音频输出: 找到混音器控制 '%s', 音量范围=[%ld, %ld]\n", 
                 ctrl_names[i], ao->vol_min, ao->vol_max);
            return 0;
        }
    }
    
    // 未找到可用控件，关闭 mixer
    LOGE("音频输出: 未找到合适的混音器控制\n");
    snd_mixer_close(ao->mixer);
    ao->mixer = NULL;
    return -1;
}

/* ---------- 公开接口实现 ---------- */

AudioOutput* audio_output_init(unsigned int sample_rate, int channels,
                               int period_size, RingBuffer *rb)
{
    AudioOutput *ao = (AudioOutput*)calloc(1, sizeof(AudioOutput));
    if (!ao) return NULL;

    ao->sample_rate = sample_rate;
    ao->channels = channels;
    ao->period_size = period_size;
    // 使用32位格式以支持WM8960的24位能力
    ao->format = SND_PCM_FORMAT_S32_LE;
    ao->rb = rb;
    ao->running = 0;
    ao->paused = 0;

    /* 打开默认声卡 */
    int ret = snd_pcm_open(&ao->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        LOGE("音频输出: snd_pcm_open 失败: %s\n", snd_strerror(ret));
        free(ao);
        return NULL;
    }

    /* 设置硬件参数 */
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(ao->handle, hw_params);

    // 访问模式：交错访问
    snd_pcm_hw_params_set_access(ao->handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // 格式：32位有符号小端（支持WM8960的24位能力，也是解码器输出的格式）
    if (snd_pcm_hw_params_set_format(ao->handle, hw_params, SND_PCM_FORMAT_S32_LE) < 0) {
        LOGE("音频输出: 无法设置32位音频格式，这可能影响WM8960的24位能力\n");
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(ao->handle);
        free(ao);
        return NULL;
    }
    ao->format = SND_PCM_FORMAT_S32_LE;
    
    // 声道数
    snd_pcm_hw_params_set_channels(ao->handle, hw_params, channels);

    // 采样率（near：使用最接近硬件支持的值）
    unsigned int rate = sample_rate;
    snd_pcm_hw_params_set_rate_near(ao->handle, hw_params, &rate, 0);
    ao->sample_rate = rate;  // 实际使用的采样率

    // period 大小（一次中断传输的帧数）
    snd_pcm_uframes_t period = period_size;
    snd_pcm_hw_params_set_period_size_near(ao->handle, hw_params, &period, 0);
    ao->period_size = (unsigned int)period;

    // 缓冲区大小（设为 period 的 4 倍，减小溢出风险）
    snd_pcm_uframes_t buffer_size = period * 8;
    snd_pcm_hw_params_set_buffer_size_near(ao->handle, hw_params, &buffer_size);

    // 应用参数
    ret = snd_pcm_hw_params(ao->handle, hw_params);
    if (ret < 0) {
        LOGE("音频输出: snd_pcm_hw_params 失败: %s\n", snd_strerror(ret));
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(ao->handle);
        free(ao);
        return NULL;
    }
    snd_pcm_hw_params_free(hw_params);

    ao->volume = 0.7f;  // 默认音量为 70%（避免声音太大）
    
    // 初始化线程退出同步机制
    pthread_mutex_init(&ao->exit_lock, NULL);
    pthread_cond_init(&ao->exit_cond, NULL);
    ao->thread_exited = 0;
    
    // 尝试初始化硬件 mixer（失败不影响音频播放，降级为软件音量）
    audio_output_init_mixer(ao);
    
    // 初始化时设置硬件音量为默认值
    if (ao->mixer) {
        audio_output_set_volume_all(ao, ao->volume);
        LOGD("音频输出: 初始音量设置为 %.2f\n", ao->volume);
    }
    
    LOGD("音频输出: 初始化成功, 采样率=%u, 声道=%d, 格式=%d, 周期=%d, 缓冲区=%u, 混音器=%s\n",
           ao->sample_rate, ao->channels, ao->format, ao->period_size, buffer_size,
           ao->mixer ? "启用" : "禁用");
    return ao;
}

void audio_output_start(AudioOutput *ao) {
    if (!ao) return;
    if (ao->running) return;  // 已启动

    // 重置 ALSA PCM 设备状态
    snd_pcm_drop(ao->handle);
    snd_pcm_prepare(ao->handle);

    ao->running = 1;
    if (pthread_create(&ao->thread, NULL, ao_thread_fn, ao) != 0) {
        LOGE("音频输出: pthread_create 失败\n");
        ao->running = 0;
    }
}

void audio_output_pause(AudioOutput *ao) {
    if (ao) ao->paused = 1;
}

void audio_output_resume(AudioOutput *ao) {
    if (ao) ao->paused = 0;
}

void audio_output_stop(AudioOutput *ao) {
    LOGD("audio_output_stop: 开始停止音频输出\n");
    if (!ao) {
        LOGD("audio_output_stop: ao为NULL，直接返回\n");
        return;
    }
    if (!ao->running) {
        LOGD("audio_output_stop: ao->running为0，已经停止，直接返回\n");
        return;
    }
    
    LOGD("音频输出: 正在停止...\n");
    
    // 通知线程退出
    LOGD("audio_output_stop: 设置running=0\n");
    ao->running = 0;

    /* 关键步骤：唤醒可能阻塞在 rb_read 的播放线程 */
    LOGD("audio_output_stop: 尝试唤醒阻塞的读取线程\n");
    RingBuffer *rb_copy = ao->rb;  // 获取环形缓冲区指针的本地副本
    if (rb_copy) {
        rb_wakeup_all(rb_copy);
    } else {
        LOGD("audio_output_stop: ao->rb为NULL\n");
    }

    // 使用通知机制等待线程结束，替代超时等待
    LOGD("audio_output_stop: 等待线程退出确认\n");
    
    // 加锁检查线程是否已经退出
    pthread_mutex_lock(&ao->exit_lock);
    if (!ao->thread_exited) {
        // 等待线程发出退出通知，设置一个较短的超时时间作为后备
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 2; // 2秒超时作为绝对上限
        
        int ret = pthread_cond_timedwait(&ao->exit_cond, &ao->exit_lock, &timeout);
        if (ret == ETIMEDOUT) {
            LOGD("audio_output: 线程退出等待超时\n");
            // 即使超时，也继续执行清理，因为可能线程已退出但未正确设置标志
        }
    }
    pthread_mutex_unlock(&ao->exit_lock);
    
    LOGD("audio_output_stop: 线程已确认退出\n");
    
    ao->running = 0;   // 确保标志清除
    
    LOGD("音频输出: 已停止\n");
}

void audio_output_free(AudioOutput *ao) {
    LOGD("audio_output_free: 开始释放音频输出\n");
    if (!ao) {
        LOGD("audio_output_free: ao为NULL，直接返回\n");
        return;
    }

    // 确保线程已退出
    LOGD("audio_output_free: 检查并停止运行中的线程\n");
    if (ao->running) {
        audio_output_stop(ao);
    } else {
        LOGD("audio_output_free: ao->running为0，无需停止线程\n");
    }

    // 关闭 mixer
    if (ao->mixer) {
        LOGD("audio_output_free: 关闭混音器\n");
        snd_mixer_close(ao->mixer);
        ao->mixer = NULL;
        LOGD("音频输出: 混音器已关闭\n");
    } else {
        LOGD("audio_output_free: ao->mixer为NULL\n");
    }

    // 在关闭PCM设备前，再次确保音频线程已完全停止
    // 这里我们不能使用join，因为线程应该已经停止了
    // 但我们需要确保句柄不再被线程访问
    
    if (ao->handle) {
        LOGD("audio_output_free: 关闭PCM设备\n");
        // 确保 PCM 设备处于停止状态
        snd_pcm_drop(ao->handle);
        // 等待硬件缓冲区清空
        snd_pcm_drain(ao->handle);
        // 关闭设备
        snd_pcm_close(ao->handle);
        ao->handle = NULL;
        LOGD("音频输出: PCM句柄已关闭\n");
    } else {
        LOGD("audio_output_free: ao->handle为NULL\n");
    }
    
    // 清除对环形缓冲区的引用，避免在销毁后仍可能被访问
    LOGD("audio_output_free: 清除环形缓冲区引用\n");
    ao->rb = NULL;

    // 销毁线程退出同步机制
    pthread_mutex_destroy(&ao->exit_lock);
    pthread_cond_destroy(&ao->exit_cond);

    LOGD("audio_output_free: 释放音频输出内存\n");
    free(ao);
    LOGD("音频输出: 已释放\n");
}

void audio_output_set_volume(AudioOutput *ao, float volume) {
    if (!ao) return;
    
    // 限制音量范围
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    ao->volume = volume;
    
    // 优先使用硬件音量控制
    if (ao->elem) {
        // 将 0.0-1.0 的音量值转换为硬件音量范围
        long hw_volume = (long)(ao->vol_min + volume * (ao->vol_max - ao->vol_min));
        
        // 确保在范围内
        if (hw_volume < ao->vol_min) hw_volume = ao->vol_min;
        if (hw_volume > ao->vol_max) hw_volume = ao->vol_max;
        
        // 分别设置左右声道音量（确保生效）
        snd_mixer_selem_set_playback_volume(ao->elem, SND_MIXER_SCHN_FRONT_LEFT, hw_volume);
        snd_mixer_selem_set_playback_volume(ao->elem, SND_MIXER_SCHN_FRONT_RIGHT, hw_volume);
        
        // 处理 mixer 事件，使音量设置立即生效
        snd_mixer_handle_events(ao->mixer);
        
        LOGD("音频输出: 硬件音量设置为 %.2f (硬件值: %ld)\n", volume, hw_volume);
    } else {
        LOGD("音频输出: 软件音量设置为 %.2f (混音器不可用)\n", volume);
    }
}

/**
 * @brief 设置所有可用的 mixer 控件音量（用于 WM8960 等需要多个控件的芯片）
 */
void audio_output_set_volume_all(AudioOutput *ao, float volume) {
    if (!ao || !ao->mixer) return;
    
    // ==== WM8960 音量控制策略 ====
    // 根据硬件特性，采用两级音量控制策略：
    // 1. Playback（数字音量）：固定在较高水平(85%)，避免大衰减
    // 2. Headphone/Speaker（硬件放大）：根据用户设置的音量进行调节
    // 这样可以获得更好的信噪比和调节线性度
    
    // 打开左右声道 PCM Mixer（野火教程要求）
    const char *enable_ctrls[] = {"Right Output Mixer PCM", "Left Output Mixer PCM", NULL};
    for (int i = 0; enable_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_name(sid, enable_ctrls[i]);
        snd_mixer_elem_t *elem = snd_mixer_find_selem(ao->mixer, sid);
        
        if (elem) {
            snd_mixer_selem_set_playback_switch_all(elem, 1);
        }
    }
    
    // Playback 固定在85%（避免数字衰减过大）
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_name(sid, "Playback");
    snd_mixer_elem_t *elem = snd_mixer_find_selem(ao->mixer, sid);
    if (elem) {
        long min, max;
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        long hw_vol = min + (max - min) * 85 / 100;  // 固定85%
        snd_mixer_selem_set_playback_volume_all(elem, hw_vol);
    }
    
    // Headphone 和 Speaker 根据用户音量设置调节（使用三次方根映射）
    float curve_volume = powf(volume, 1.0f / 3.0f);
    int vol_percent = (int)(curve_volume * 100);
    
    // 最低音量阈值（20%）- WM8960在低音量时dB衰减很大
    if (vol_percent > 0 && vol_percent < 20) vol_percent = 20;
    if (vol_percent < 0) vol_percent = 0;
    if (vol_percent > 100) vol_percent = 100;
    
    const char *hw_vol_ctrls[] = {"Headphone", "Speaker", NULL};
    for (int i = 0; hw_vol_ctrls[i]; i++) {
        snd_mixer_selem_id_t *sid_hw;
        snd_mixer_selem_id_alloca(&sid_hw);
        snd_mixer_selem_id_set_name(sid_hw, hw_vol_ctrls[i]);
        snd_mixer_elem_t *elem_hw = snd_mixer_find_selem(ao->mixer, sid_hw);
        
        if (elem_hw) {
            long min, max;
            snd_mixer_selem_get_playback_volume_range(elem_hw, &min, &max);
            long hw_vol = min + (max - min) * vol_percent / 100;
            snd_mixer_selem_set_playback_volume_all(elem_hw, hw_vol);
        }
    }
    
    // 处理 mixer 事件
    snd_mixer_handle_events(ao->mixer);
}

float audio_output_get_volume(AudioOutput *ao) {
    return ao ? ao->volume : 1.0f;
}