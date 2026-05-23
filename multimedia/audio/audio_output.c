#include "audio_output.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libavutil/samplefmt.h>

/* ---------- 内部播放线程 ---------- */
static void* ao_thread_fn(void *arg) {
    AudioOutput *ao = (AudioOutput*)arg;
    int frame_bytes = ao->channels * sizeof(int16_t);       // 每帧字节数
    int buf_frames = ao->period_size;                       // 缓冲帧数
    int buf_bytes  = buf_frames * frame_bytes;
    int16_t *buf = (int16_t*)malloc(buf_bytes);
    if (!buf) {
        LOGE("audio output: malloc failed\n");
        return NULL;
    }

    while (ao->running) {
        /* ---------- 暂停处理 ---------- */
        if (ao->paused) {
            usleep(10000);          // 10ms 睡眠，防止 CPU 空转
            continue;
        }

        /* ---------- 从环形缓冲区读取一帧数据 ---------- */
        int ret = rb_read(ao->rb, (uint8_t*)buf, buf_bytes);
        if (ret < 0) {
            // 缓冲区被销毁或发生错误，退出线程
            break;
        }
        // ret 可能小于 buf_bytes （正常阻塞读取会等够）
        if (ret == 0) continue;

        /* ---------- 写入 ALSA 声卡 ---------- */
        int frames_to_write = ret / frame_bytes;
        int written = snd_pcm_writei(ao->handle, buf, frames_to_write);
        if (written < 0) {
            // 发生 XRUN 或其它错误，尝试恢复
            LOGE("audio output: XRUN recover, written = %d\n", written);
            snd_pcm_recover(ao->handle, written, 1);
            // 跳过此次迭代，重新读取数据
            continue;
        }
    }

    free(buf);
    // 线程退出前清空声卡缓冲区
    snd_pcm_drain(ao->handle);
    return NULL;
}

/* ---------- 尝试打开并设置 mixer（支持多种常见控件名称） ---------- */
static int audio_output_init_mixer(AudioOutput *ao) {
    const char *ctrl_names[] = {"Master", "PCM", "Headphone", "Speaker", "Playback", NULL};
    int ret;
    
    // 打开 mixer
    ret = snd_mixer_open(&ao->mixer, 0);
    if (ret < 0) {
        LOGE("audio_output: snd_mixer_open failed: %s\n", snd_strerror(ret));
        return -1;
    }
    
    // 附加默认声卡
    ret = snd_mixer_attach(ao->mixer, "default");
    if (ret < 0) {
        LOGE("audio_output: snd_mixer_attach failed: %s\n", snd_strerror(ret));
        snd_mixer_close(ao->mixer);
        ao->mixer = NULL;
        return -1;
    }
    
    // 注册元素类型
    snd_mixer_selem_register(ao->mixer, NULL, NULL);
    
    // 加载 mixer 元素
    ret = snd_mixer_load(ao->mixer);
    if (ret < 0) {
        LOGE("audio_output: snd_mixer_load failed: %s\n", snd_strerror(ret));
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
            LOGD("audio_output: found mixer control '%s', vol_range=[%ld, %ld]\n", 
                 ctrl_names[i], ao->vol_min, ao->vol_max);
            return 0;
        }
    }
    
    // 未找到可用控件，关闭 mixer
    LOGE("audio_output: no suitable mixer control found\n");
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
    // 使用固定16位格式（简单可靠，兼容性好）
    ao->format = SND_PCM_FORMAT_S16_LE;
    ao->rb = rb;
    ao->running = 0;
    ao->paused = 0;

    /* 打开默认声卡 */
    int ret = snd_pcm_open(&ao->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        LOGE("audio_output: snd_pcm_open failed: %s\n", snd_strerror(ret));
        free(ao);
        return NULL;
    }

    /* 设置硬件参数 */
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(ao->handle, hw_params);

    // 访问模式：交错访问
    snd_pcm_hw_params_set_access(ao->handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    // 格式：固定16位有符号小端
    snd_pcm_hw_params_set_format(ao->handle, hw_params, SND_PCM_FORMAT_S16_LE);
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
        LOGE("audio_output: snd_pcm_hw_params failed: %s\n", snd_strerror(ret));
        snd_pcm_hw_params_free(hw_params);
        snd_pcm_close(ao->handle);
        free(ao);
        return NULL;
    }
    snd_pcm_hw_params_free(hw_params);

    ao->volume = 0.7f;  // 默认音量为 70%（避免声音太大）
    
    // 尝试初始化硬件 mixer（失败不影响音频播放，降级为软件音量）
    audio_output_init_mixer(ao);
    
    // 初始化时设置硬件音量为默认值
    if (ao->mixer) {
        audio_output_set_volume_all(ao, ao->volume);
        LOGD("audio_output: initial volume set to %.2f\n", ao->volume);
    }
    
    LOGD("audio_output: init OK, rate=%u, ch=%d, fmt=%d, period=%d, buffer=%u, mixer=%s\n",
           ao->sample_rate, ao->channels, ao->format, ao->period_size, buffer_size,
           ao->mixer ? "enabled" : "disabled");
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
        LOGE("audio_output: pthread_create failed\n");
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
    if (!ao) return;
    if (!ao->running) return;

    LOGD("audio_output: stopping...\n");
    
    // 通知线程退出
    ao->running = 0;

    /* 关键步骤：唤醒可能阻塞在 rb_read 的播放线程 */
    if (ao->rb) {
        rb_wakeup_all(ao->rb);
    }

    // 等待线程结束
    pthread_join(ao->thread, NULL);
    ao->running = 0;   // 确保标志清除
    
    LOGD("audio_output: stopped\n");
}

void audio_output_free(AudioOutput *ao) {
    if (!ao) return;

    // 确保线程已退出
    if (ao->running) {
        audio_output_stop(ao);
    }

    if (ao->handle) {
        // 确保 PCM 设备处于停止状态
        snd_pcm_drop(ao->handle);
        // 等待硬件缓冲区清空
        snd_pcm_drain(ao->handle);
        // 关闭设备
        snd_pcm_close(ao->handle);
        ao->handle = NULL;
        LOGD("audio_output: PCM handle closed\n");
    }
    
    // 关闭 mixer
    if (ao->mixer) {
        snd_mixer_close(ao->mixer);
        ao->mixer = NULL;
        LOGD("audio_output: mixer closed\n");
    }

    free(ao);
    LOGD("audio_output: freed\n");
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
        
        LOGD("audio_output: hardware volume set to %.2f (hw: %ld)\n", volume, hw_volume);
    } else {
        LOGD("audio_output: software volume set to %.2f (mixer not available)\n", volume);
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