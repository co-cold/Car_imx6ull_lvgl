#define _GNU_SOURCE
#include "audio_decoder.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../utils/debug.h"

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>

// 为WM8960优化的输出参数（支持24位，但保持原生采样率）
#define OUT_CHANNELS    2

static void* ad_thread_fn(void *arg) {
    AudioDecoder *ad = (AudioDecoder*)arg;
    LOGD("音频解码线程已启动\n");
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!pkt || !frame) {
        LOGD("音频解码器: 线程退出 - 内存分配失败\n");
        return NULL;
    }

    while (1) {
        // 检查退出标志
        if (!ad->running) break;
        
        // 处理跳转
        if (ad->seek_req >= 0) {
            int64_t seek_target = ad->seek_req;
            ad->seek_req = -1;
            int64_t pts = av_rescale_q(seek_target * (AV_TIME_BASE / 1000),
                                       AV_TIME_BASE_Q,
                                       ad->fmt_ctx->streams[ad->audio_stream_idx]->time_base);
            av_seek_frame(ad->fmt_ctx, ad->audio_stream_idx, pts, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(ad->codec_ctx);
            rb_reset(ad->rb);
        }

        // 检查退出标志
        if (!ad->running) break;
        
        int ret = av_read_frame(ad->fmt_ctx, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF) break;
            continue;
        }
        if (pkt->stream_index != ad->audio_stream_idx) {
            av_packet_unref(pkt);
            continue;
        }

        // 检查退出标志
        if (!ad->running) break;
        
        ret = avcodec_send_packet(ad->codec_ctx, pkt);
        if (ret < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (1) {
            // 检查退出标志
            if (!ad->running) break;
            
            ret = avcodec_receive_frame(ad->codec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
            if (ret < 0) {
                av_frame_unref(frame);
                break;
            }

            // 计算重采样输出样本数（输出采样率等于输入采样率）
            int out_samples = swr_get_delay(ad->swr_ctx, frame->sample_rate) + frame->nb_samples;

            // 根据WM8960支持的格式选择输出格式，优先使用24位
            enum AVSampleFormat out_fmt = AV_SAMPLE_FMT_S32;  // 使用32位整数以支持24位精度

            // 动态分配输出缓冲区
            uint8_t **out_buf = NULL;
            int linesize;
            ret = av_samples_alloc_array_and_samples(&out_buf, &linesize,
                                                     OUT_CHANNELS, out_samples,
                                                     out_fmt, 0);
            if (ret < 0) {
                av_frame_unref(frame);
                break;
            }

            // 重采样（只进行必要的格式转换，保持原生采样率）
            int converted = swr_convert(ad->swr_ctx, out_buf, out_samples,
                                        (const uint8_t **)frame->data,
                                        frame->nb_samples);
            if (converted > 0) {
                int bytes_per_sample = av_get_bytes_per_sample(out_fmt);
                int out_bytes = converted * OUT_CHANNELS * bytes_per_sample;
                
                // 检查退出标志
                if (!ad->running) break;
                
                // 防御性检查：确保 out_buf[0] 非空
                RingBuffer *rb_copy = ad->rb;  // 获取环形缓冲区指针的本地副本
                if (out_buf[0] && rb_copy) {
                    rb_write(rb_copy, out_buf[0], out_bytes);
                }
                
                // 获取帧时间戳（兼容旧版本 FFmpeg）
                int64_t pts = frame->pts;
                if (pts == AV_NOPTS_VALUE && frame->pkt_dts != AV_NOPTS_VALUE) {
                    pts = frame->pkt_dts;
                }
                if (pts != AV_NOPTS_VALUE) {
                    AVRational tb = ad->fmt_ctx->streams[ad->audio_stream_idx]->time_base;
                    ad->current_time = pts * av_q2d(tb);
                    
                    // 音画同步已移除，仅保留时间更新
                }
            }

            // 正确释放：先释放数据，再释放指针数组
            av_freep(&out_buf[0]);
            av_freep(&out_buf);
            av_frame_unref(frame);
        }
        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    
    // 设置线程退出标志并发出通知
    pthread_mutex_lock(&ad->exit_lock);
    ad->thread_exited = 1;
    pthread_cond_broadcast(&ad->exit_cond);
    pthread_mutex_unlock(&ad->exit_lock);
    
    LOGD("音频解码线程退出\n");
    return NULL;
}

AudioDecoder* audio_decoder_init(const char *file_path, RingBuffer *rb) {
    AudioDecoder *ad = calloc(1, sizeof(AudioDecoder));
    if (!ad) return NULL;

    ad->file_path = strdup(file_path);
    ad->rb = rb;
    ad->running = 0;
    ad->seek_req = -1;
    ad->duration = 0;
    ad->current_time = 0;

    // 打开文件
    if (avformat_open_input(&ad->fmt_ctx, file_path, NULL, NULL) < 0) {
        LOGE("audio_decoder: cannot open input\n");
        goto fail;
    }
    if (avformat_find_stream_info(ad->fmt_ctx, NULL) < 0) {
        LOGE("audio_decoder: cannot find stream info\n");
        goto fail;
    }
    // 找音频流
    ad->audio_stream_idx = av_find_best_stream(ad->fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ad->audio_stream_idx < 0) {
        LOGE("audio_decoder: no audio stream\n");
        goto fail;
    }
    AVCodecParameters *par = ad->fmt_ctx->streams[ad->audio_stream_idx]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(par->codec_id);
    if (!codec) {
        LOGE("audio_decoder: unsupported codec\n");
        goto fail;
    }
    ad->codec_ctx = avcodec_alloc_context3(codec);
    if (!ad->codec_ctx) goto fail;
    if (avcodec_parameters_to_context(ad->codec_ctx, par) < 0) goto fail;
    if (avcodec_open2(ad->codec_ctx, codec, NULL) < 0) {
        LOGE("audio_decoder: cannot open codec\n");
        goto fail;
    }

    // 保存采样率、声道数
    ad->sample_rate = ad->codec_ctx->sample_rate;
    ad->channels = ad->codec_ctx->ch_layout.nb_channels;

    // 配置重采样器（使用 swr_alloc_set_opts2）
    // 输出为WM8960兼容的格式（24位/32位，立体声），但保持输入的原生采样率以保持音质
    AVChannelLayout out_ch_layout, in_ch_layout;
    // 使用固定的立体声布局作为输出布局（WM8960兼容）
    av_channel_layout_default(&out_ch_layout, OUT_CHANNELS);
    
    // 使用输入文件的声道布局作为输入布局
    if (ad->codec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
        av_channel_layout_default(&in_ch_layout, ad->codec_ctx->ch_layout.nb_channels);
    else
        in_ch_layout = ad->codec_ctx->ch_layout;

    // 输出为32位格式以支持24位精度，但保持输入文件的原生采样率
    if (swr_alloc_set_opts2(&ad->swr_ctx,
                            &out_ch_layout, AV_SAMPLE_FMT_S32, ad->codec_ctx->sample_rate,
                            &in_ch_layout, ad->codec_ctx->sample_fmt,
                            ad->codec_ctx->sample_rate,
                            0, NULL) < 0) {
        LOGE("音频解码器: swr_alloc_set_opts2 失败\n");
        goto fail;
    }
    if (swr_init(ad->swr_ctx) < 0) {
        LOGE("audio_decoder: swr_init failed\n");
        goto fail;
    }

    // 初始化线程退出同步机制
    pthread_mutex_init(&ad->exit_lock, NULL);
    pthread_cond_init(&ad->exit_cond, NULL);
    ad->thread_exited = 0;
    
    // 时长
    if (ad->fmt_ctx->duration != AV_NOPTS_VALUE) {
        ad->duration = (double)ad->fmt_ctx->duration / AV_TIME_BASE;
    }
    LOGD("audio_decoder: init OK, %s\n", file_path);
    return ad;

fail:
    audio_decoder_free(ad);
    return NULL;
}

void audio_decoder_start(AudioDecoder *ad) {
    if (!ad || ad->running) return;
    ad->running = 1;
    if (pthread_create(&ad->thread, NULL, ad_thread_fn, ad) != 0) {
        LOGE("audio_decoder: pthread_create failed\n");
        ad->running = 0;
    }
}

void audio_decoder_stop(AudioDecoder *ad) {
    LOGD("audio_decoder_stop: 开始停止音频解码器\n");
    if (!ad) {
        LOGD("audio_decoder_stop: ad为NULL，直接返回\n");
        return;
    }
    if (!ad->running) {
        LOGD("audio_decoder_stop: ad->running为0，已经停止，直接返回\n");
        return;
    }
    
    ad->running = 0;
    
    // 唤醒可能阻塞在 rb_write 的线程
    LOGD("audio_decoder_stop: 尝试唤醒阻塞的写入线程\n");
    RingBuffer *rb_copy = ad->rb;  // 获取环形缓冲区指针的本地副本
    if (rb_copy) {
        rb_reset(rb_copy);
        // 检查缓冲区是否还在有效状态才写入唤醒数据
        if (!(rb_copy->abort_flag || rb_copy->destroy_in_progress)) {
            LOGD("audio_decoder_stop: 向环形缓冲区写入唤醒数据\n");
            uint8_t dummy = 0;
            rb_write(rb_copy, &dummy, 1);
        } else {
            LOGD("audio_decoder_stop: 环形缓冲区已标记为销毁，跳过唤醒写入\n");
        }
    } else {
        LOGD("audio_decoder_stop: ad->rb为NULL\n");
    }
    
    // 使用通知机制等待线程结束，替代超时等待
    LOGD("audio_decoder_stop: 等待解码线程退出确认\n");
    
    // 加锁检查线程是否已经退出
    pthread_mutex_lock(&ad->exit_lock);
    if (!ad->thread_exited) {
        // 等待线程发出退出通知，设置一个较短的超时时间作为后备
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 2; // 2秒超时作为绝对上限
        
        int ret = pthread_cond_timedwait(&ad->exit_cond, &ad->exit_lock, &timeout);
        if (ret == ETIMEDOUT) {
            LOGD("audio_decoder: 线程退出等待超时\n");
            // 即使超时，也继续执行清理，因为可能线程已退出但未正确设置标志
        }
    }
    pthread_mutex_unlock(&ad->exit_lock);
    
    LOGD("audio_decoder_stop: 解码线程已确认退出\n");
    
    ad->running = 0;   // 确保标志清除
    LOGD("audio_decoder_stop: 完成音频解码器停止\n");
}

void audio_decoder_seek(AudioDecoder *ad, double seconds) {
    if (ad) ad->seek_req = (int64_t)(seconds * 1000);
}

double audio_decoder_get_position(AudioDecoder *ad) {
    if (!ad) return 0.0;
    return ad->current_time;
}

void audio_decoder_free(AudioDecoder *ad) {
    LOGD("audio_decoder_free: 开始释放音频解码器\n");
    if (!ad) {
        LOGD("audio_decoder_free: ad为NULL，直接返回\n");
        return;
    }
    if (ad->running) {
        LOGD("audio_decoder_free: 解码器仍在运行，先停止\n");
        audio_decoder_stop(ad);
    } else {
        LOGD("audio_decoder_free: 解码器未运行，无需停止\n");
    }
    LOGD("audio_decoder_free: 释放FFmpeg资源\n");
    swr_free(&ad->swr_ctx);
    avcodec_free_context(&ad->codec_ctx);
    avformat_close_input(&ad->fmt_ctx);
    free(ad->file_path);
    
    // 清除对环形缓冲区的引用，避免在销毁后仍可能被访问
    LOGD("audio_decoder_free: 清除环形缓冲区引用\n");
    ad->rb = NULL;
    
    // 销毁线程退出同步机制
    pthread_mutex_destroy(&ad->exit_lock);
    pthread_cond_destroy(&ad->exit_cond);
    
    LOGD("audio_decoder_free: 释放音频解码器内存\n");
    free(ad);
    LOGD("audio_decoder_free: 完成音频解码器释放\n");
}