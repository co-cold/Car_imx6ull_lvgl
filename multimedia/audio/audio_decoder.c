#include "audio_decoder.h"
#include <stdlib.h>
#include <string.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>

// 输出目标参数（使用固定16位格式，简单可靠）
#define OUT_CHANNELS    2
#define OUT_SAMPLE_FMT  AV_SAMPLE_FMT_S16

static void* ad_thread_fn(void *arg) {
    AudioDecoder *ad = (AudioDecoder*)arg;
    LOGD("thread started\n");
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!pkt || !frame) {
        LOGD("audio_decoder: thread exiting - alloc failed\n");
        return NULL;
    }

    while (ad->running) {
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

        int ret = av_read_frame(ad->fmt_ctx, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF) break;
            continue;
        }
        if (pkt->stream_index != ad->audio_stream_idx) {
            av_packet_unref(pkt);
            continue;
        }

        ret = avcodec_send_packet(ad->codec_ctx, pkt);
        if (ret < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (1) {
            ret = avcodec_receive_frame(ad->codec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
            if (ret < 0) {
                av_frame_unref(frame);
                break;
            }

            // 计算重采样输出样本数（输出采样率等于输入采样率）
            int out_samples = swr_get_delay(ad->swr_ctx, frame->sample_rate) + frame->nb_samples;

            // 使用16位格式输出
            enum AVSampleFormat out_fmt = OUT_SAMPLE_FMT;

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

            // 重采样
            int converted = swr_convert(ad->swr_ctx, out_buf, out_samples,
                                        (const uint8_t **)frame->data,
                                        frame->nb_samples);
            if (converted > 0) {
                int bytes_per_sample = av_get_bytes_per_sample(out_fmt);
                int out_bytes = converted * OUT_CHANNELS * bytes_per_sample;
                
                // 对于高比特深度格式转换到16位，应用增益放大
                // 24位FLAC转换到16位时动态范围被压缩，需要适当放大
                if (frame->format == AV_SAMPLE_FMT_FLTP || 
                    frame->format == AV_SAMPLE_FMT_FLT ||
                    frame->format == AV_SAMPLE_FMT_S32 ||
                    frame->format == AV_SAMPLE_FMT_S32P) {
                    int16_t *samples = (int16_t *)out_buf[0];
                    int num_samples = out_bytes / sizeof(int16_t);
                    float gain = 2.0f; // 适当提高音量
                    for (int i = 0; i < num_samples; i++) {
                        int32_t val = (int32_t)(samples[i] * gain);
                        // 限制在16位范围内
                        if (val > INT16_MAX) val = INT16_MAX;
                        if (val < INT16_MIN) val = INT16_MIN;
                        samples[i] = (int16_t)val;
                    }
                }
                
                // 防御性检查：确保 out_buf[0] 非空
                if (out_buf[0]) {
                    rb_write(ad->rb, out_buf[0], out_bytes);
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
    LOGD("thread exiting\n");
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
    // 输出采样率和格式都使用文件原生参数，避免不必要的转换
    AVChannelLayout out_ch_layout, in_ch_layout;
    av_channel_layout_default(&out_ch_layout, OUT_CHANNELS);
    // 输入声道布局尝试从解码器上下文获取，否则用立体声
    if (ad->codec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
        av_channel_layout_default(&in_ch_layout, OUT_CHANNELS);
    else
        in_ch_layout = ad->codec_ctx->ch_layout;

    // 使用16位格式输出
    if (swr_alloc_set_opts2(&ad->swr_ctx,
                            &out_ch_layout, OUT_SAMPLE_FMT, ad->codec_ctx->sample_rate,
                            &in_ch_layout, ad->codec_ctx->sample_fmt,
                            ad->codec_ctx->sample_rate,
                            0, NULL) < 0) {
        LOGE("audio_decoder: swr_alloc_set_opts2 failed\n");
        goto fail;
    }
    if (swr_init(ad->swr_ctx) < 0) {
        LOGE("audio_decoder: swr_init failed\n");
        goto fail;
    }

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
    if (!ad || !ad->running) return;
    ad->running = 0;
    // 唤醒可能阻塞在 rb_write 的线程
    rb_reset(ad->rb);
    uint8_t dummy = 0;
    rb_write(ad->rb, &dummy, 1);
    pthread_join(ad->thread, NULL);
}

void audio_decoder_seek(AudioDecoder *ad, double seconds) {
    if (ad) ad->seek_req = (int64_t)(seconds * 1000);
}

double audio_decoder_get_position(AudioDecoder *ad) {
    if (!ad) return 0.0;
    return ad->current_time;
}

void audio_decoder_free(AudioDecoder *ad) {
    if (!ad) return;
    if (ad->running) audio_decoder_stop(ad);
    swr_free(&ad->swr_ctx);
    avcodec_free_context(&ad->codec_ctx);
    avformat_close_input(&ad->fmt_ctx);
    free(ad->file_path);
    free(ad);
}