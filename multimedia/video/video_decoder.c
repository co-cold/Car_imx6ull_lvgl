#define _GNU_SOURCE
#include "video_decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "../utils/debug.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>

static void* vd_thread_fn(void *arg) {
    VideoDecoder *vd = (VideoDecoder*)arg;
    LOGD("video_decoder: thread started for %s\n", vd->file_path);
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!pkt || !frame) {
        LOGD("video_decoder: thread exiting - alloc failed\n");
        return NULL;
    }

    int render_count = 0;
    double frame_interval = vd->fps > 0 ? (1.0 / vd->fps) : 0.0416;
    struct timespec last_real_ts = {0, 0};

    while (vd->running) {
        while (vd->paused && vd->running) {
            av_usleep(10000);
        }

        if (!vd->running) break;

        int ret = av_read_frame(vd->fmt_ctx, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF) break;
            av_packet_unref(pkt);
            continue;
        }

        if (!vd->running) {
            av_packet_unref(pkt);
            break;
        }

        if (pkt->stream_index != vd->video_stream_idx) {
            av_packet_unref(pkt);
            continue;
        }

        ret = avcodec_send_packet(vd->codec_ctx, pkt);
        if (ret < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (1) {
            if (!vd->running) {
                av_frame_unref(frame);
                break;
            }

            ret = avcodec_receive_frame(vd->codec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
            if (ret < 0) {
                av_frame_unref(frame);
                break;
            }

            if (!vd->running) {
                av_frame_unref(frame);
                break;
            }

            VideoDoubleBuf *vdb_copy = vd->vdb;
            if (!vdb_copy) {
                LOGD("video_decoder: vdb已释放，退出线程\n");
                av_frame_unref(frame);
                break;
            }
            uint8_t *dst = vdb_get_write_buf(vdb_copy);
            if (!dst) {
                av_frame_unref(frame);
                continue;
            }

            render_count++;
            if (render_count % 300 == 0) {
                LOGD("video_decoder: rendered %d frames\n", render_count);
            }

            uint8_t *dest[1] = { dst };
            int dst_linesize[1] = { vd->dst_width * 2 };
            sws_scale(vd->sws_ctx,
                      (const uint8_t *const *)frame->data, frame->linesize,
                      0, frame->height,
                      dest, dst_linesize);

            double frame_pts = (frame->pts != AV_NOPTS_VALUE)
                ? frame->pts * av_q2d(vd->fmt_ctx->streams[vd->video_stream_idx]->time_base)
                : -1.0;

            vdb_commit_with_pts(vdb_copy, frame_pts);
            av_frame_unref(frame);

            if (last_real_ts.tv_sec != 0 || last_real_ts.tv_nsec != 0) {
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);
                double wall_elapsed = (now.tv_sec - last_real_ts.tv_sec)
                    + (now.tv_nsec - last_real_ts.tv_nsec) * 1e-9;
                if (wall_elapsed < frame_interval && wall_elapsed > 0) {
                    av_usleep((unsigned int)((frame_interval - wall_elapsed) * 1000000));
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &last_real_ts);
        }
        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);

    pthread_mutex_lock(&vd->exit_lock);
    vd->thread_exited = 1;
    pthread_cond_broadcast(&vd->exit_cond);
    pthread_mutex_unlock(&vd->exit_lock);

    LOGD("video_decoder: thread exiting for %s\n", vd->file_path);
    return NULL;
}

// i.MX6ULL 硬件加速支持的视频格式检查
static int is_codec_supported(AVCodecParameters *par) {
    switch (par->codec_id) {
        case AV_CODEC_ID_H264: {
            if (par->profile == 66 || par->profile == 77) {
                LOGD("video_decoder: H.264 %s Profile supported\n", 
                     par->profile == 66 ? "Baseline" : "Main");
                return 1;
            } else {
                LOGE("video_decoder: H.264 High Profile not supported on i.MX6ULL\n");
                return 0;
            }
        }
        case AV_CODEC_ID_MPEG4: {
            if (par->profile <= 1) {
                LOGD("video_decoder: MPEG-4 Simple Profile supported\n");
                return 1;
            } else {
                LOGE("video_decoder: MPEG-4 profile %d not supported on i.MX6ULL\n", par->profile);
                return 0;
            }
        }
        case AV_CODEC_ID_H263:
        case AV_CODEC_ID_MJPEG:
            LOGD("video_decoder: %s supported\n", 
                 avcodec_get_name(par->codec_id));
            return 1;
        default:
            LOGE("video_decoder: codec %d not supported on i.MX6ULL\n", par->codec_id);
            return 0;
    }
}

static int is_resolution_supported(int width, int height, double fps) {
    const int MAX_WIDTH = 1920;
    const int MAX_HEIGHT = 1080;
    const double MAX_FPS = 30.0;
    
    if (width > MAX_WIDTH || height > MAX_HEIGHT) {
        LOGE("video_decoder: resolution %dx%d exceeds i.MX6ULL limit (%dx%d)\n",
             width, height, MAX_WIDTH, MAX_HEIGHT);
        return 0;
    }
    
    if (fps > MAX_FPS) {
        LOGE("video_decoder: fps %.2f exceeds i.MX6ULL limit (%.0ffps)\n", fps, MAX_FPS);
        return 0;
    }
    
    return 1;
}

VideoDecoder* video_decoder_init(const char *file_path, VideoDoubleBuf *vdb,
                                 int dst_width, int dst_height) {
    VideoDecoder *vd = calloc(1, sizeof(VideoDecoder));
    if (!vd) return NULL;

    vd->file_path = strdup(file_path);
    vd->vdb = vdb;
    vd->dst_width = dst_width;
    vd->dst_height = dst_height;

    LOGD("video_decoder: opening %s\n", file_path);
    
    if (avformat_open_input(&vd->fmt_ctx, file_path, NULL, NULL) < 0) {
        LOGE("video_decoder: cannot open input\n");
        goto fail;
    }
    if (avformat_find_stream_info(vd->fmt_ctx, NULL) < 0) {
        fprintf(stderr, "video_decoder: cannot find stream info\n");
        goto fail;
    }
    vd->video_stream_idx = av_find_best_stream(vd->fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (vd->video_stream_idx < 0) {
        fprintf(stderr, "video_decoder: no video stream\n");
        goto fail;
    }
    AVCodecParameters *par = vd->fmt_ctx->streams[vd->video_stream_idx]->codecpar;
    
    // 检查编码格式是否支持
    if (!is_codec_supported(par)) {
        fprintf(stderr, "video_decoder: unsupported codec format for i.MX6ULL\n");
        goto fail;
    }
    
    // 获取帧率
    AVRational rate = vd->fmt_ctx->streams[vd->video_stream_idx]->avg_frame_rate;
    double fps = av_q2d(rate);
    if (fps <= 0) fps = 25.0;
    
    // 检查分辨率和帧率
    if (!is_resolution_supported(par->width, par->height, fps)) {
        fprintf(stderr, "video_decoder: resolution or fps exceeds i.MX6ULL capability\n");
        goto fail;
    }
    
    const AVCodec *codec = avcodec_find_decoder(par->codec_id);
    if (!codec) {
        fprintf(stderr, "video_decoder: unsupported codec\n");
        goto fail;
    }
    vd->codec_ctx = avcodec_alloc_context3(codec);
    if (!vd->codec_ctx) goto fail;
    if (avcodec_parameters_to_context(vd->codec_ctx, par) < 0) goto fail;
    if (avcodec_open2(vd->codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "video_decoder: cannot open codec\n");
        goto fail;
    }

    vd->sws_ctx = sws_getContext(par->width, par->height, par->format,
                                 dst_width, dst_height, AV_PIX_FMT_RGB565,
                                 SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (!vd->sws_ctx) goto fail;

    // 初始化线程退出同步机制
    pthread_mutex_init(&vd->exit_lock, NULL);
    pthread_cond_init(&vd->exit_cond, NULL);
    vd->thread_exited = 0;

    // 获取帧率
    vd->fps = fps;

    printf("video_decoder: init OK, %s, %dx%d@%.2ffps\n", file_path, par->width, par->height, vd->fps);
    return vd;

fail:
    video_decoder_free(vd);
    return NULL;
}

void video_decoder_start(VideoDecoder *vd) {
    if (!vd || vd->running) return;
    vd->running = 1;
    printf("video_decoder: starting thread for %s\n", vd->file_path);
    if (pthread_create(&vd->thread, NULL, vd_thread_fn, vd) != 0) {
        fprintf(stderr, "video_decoder: pthread_create failed\n");
        vd->running = 0;
        // 设置线程已退出标志，因为线程创建失败，实际上并没有运行
        pthread_mutex_lock(&vd->exit_lock);
        vd->thread_exited = 1;
        pthread_cond_broadcast(&vd->exit_cond);
        pthread_mutex_unlock(&vd->exit_lock);
    }
}

void video_decoder_stop(VideoDecoder *vd) {
    if (!vd || !vd->running) return;
    vd->running = 0;
    
    // 使用通知机制等待线程结束，替代超时等待
    LOGD("video_decoder_stop: 等待视频解码线程退出确认\n");
    
    // 加锁检查线程是否已经退出
    pthread_mutex_lock(&vd->exit_lock);
    if (!vd->thread_exited) {
        // 等待线程发出退出通知，设置一个较短的超时时间作为后备
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 2; // 2秒超时作为绝对上限
        
        int ret = pthread_cond_timedwait(&vd->exit_cond, &vd->exit_lock, &timeout);
        if (ret == ETIMEDOUT) {
            LOGD("video_decoder: 线程退出等待超时\n");
            // 即使超时，也继续执行清理，因为可能线程已退出但未正确设置标志
        }
    }
    pthread_mutex_unlock(&vd->exit_lock);
    
    LOGD("video_decoder_stop: 视频解码线程已确认退出\n");
}

void video_decoder_seek(VideoDecoder *vd, double seconds) {
}

void video_decoder_pause(VideoDecoder *vd) {
    if (vd) {
        vd->paused = 1;
    }
}

void video_decoder_resume(VideoDecoder *vd) {
    if (vd) {
        vd->paused = 0;
    }
}

void video_decoder_free(VideoDecoder *vd) {
    if (!vd) return;
    video_decoder_stop(vd);
    if (vd->sws_ctx) sws_freeContext(vd->sws_ctx);
    if (vd->codec_ctx) avcodec_free_context(&vd->codec_ctx);
    if (vd->fmt_ctx) avformat_close_input(&vd->fmt_ctx);
    if (vd->file_path) free(vd->file_path);
    
    // 销毁线程退出同步机制
    pthread_mutex_destroy(&vd->exit_lock);
    pthread_cond_destroy(&vd->exit_cond);
    
    free(vd);
}