#include "video_decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>

static void* vd_thread_fn(void *arg) {
    VideoDecoder *vd = (VideoDecoder*)arg;
    printf("video_decoder: thread started for %s\n", vd->file_path);
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!pkt || !frame) {
        printf("video_decoder: thread exiting - alloc failed\n");
        return NULL;
    }

    int render_count = 0;

    while (vd->running) {
        // 检查暂停状态
        while (vd->paused && vd->running) {
            av_usleep(10000);
        }
        
        // 检查停止标志（在读取帧之前）
        if (!vd->running) break;
        
        // 读取帧
        int ret = av_read_frame(vd->fmt_ctx, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF) break;
            av_packet_unref(pkt);
            continue;
        }
        
        // 检查停止标志（在处理帧之前）
        if (!vd->running) {
            av_packet_unref(pkt);
            break;
        }
        
        if (pkt->stream_index != vd->video_stream_idx) {
            av_packet_unref(pkt);
            continue;
        }

        // 解码帧
        ret = avcodec_send_packet(vd->codec_ctx, pkt);
        if (ret < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (1) {
            // 检查是否需要停止
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

            // 不做帧率控制，让视频尽可能快地解码
            // 显示帧率由LVGL定时器控制

            // 检查是否需要停止
            if (!vd->running) {
                av_frame_unref(frame);
                break;
            }
            
            // 跳帧：如果上一帧UI还没取走，直接丢弃
            uint8_t *dst = vdb_get_write_buf(vd->vdb);
            if (!dst) {
                printf("video_decoder: buffer full, dropping frame\n");
                av_frame_unref(frame);
                continue;
            }
            
            // 打印渲染信息（每300帧，减少输出频率）
            render_count++;
            if (render_count % 300 == 0) {
                printf("video_decoder: rendered %d frames\n", render_count);
            }

            // 缩放并转换为RGB565格式
            uint8_t *dest[1] = { dst };
            int dst_linesize[1] = { vd->dst_width * 2 };
            sws_scale(vd->sws_ctx,
                      (const uint8_t *const *)frame->data, frame->linesize,
                      0, frame->height,
                      dest, dst_linesize);

            // 提交帧到双缓冲
            vdb_commit(vd->vdb);
            av_frame_unref(frame);
        }
        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    printf("video_decoder: thread exiting for %s\n", vd->file_path);
    return NULL;
}

// i.MX6ULL 硬件加速支持的视频格式检查
static int is_codec_supported(AVCodecParameters *par) {
    switch (par->codec_id) {
        case AV_CODEC_ID_H264: {
            if (par->profile == 66 || par->profile == 77) {
                printf("video_decoder: H.264 %s Profile supported\n", 
                       par->profile == 66 ? "Baseline" : "Main");
                return 1;
            } else {
                fprintf(stderr, "video_decoder: H.264 High Profile not supported on i.MX6ULL\n");
                return 0;
            }
        }
        case AV_CODEC_ID_MPEG4: {
            if (par->profile <= 1) {
                printf("video_decoder: MPEG-4 Simple Profile supported\n");
                return 1;
            } else {
                fprintf(stderr, "video_decoder: MPEG-4 profile %d not supported on i.MX6ULL\n", par->profile);
                return 0;
            }
        }
        case AV_CODEC_ID_H263:
        case AV_CODEC_ID_MJPEG:
            printf("video_decoder: %s supported\n", 
                   par->codec_id == AV_CODEC_ID_H263 ? "H.263" : "MJPEG");
            return 1;
        default:
            fprintf(stderr, "video_decoder: codec %d not supported on i.MX6ULL\n", par->codec_id);
            return 0;
    }
}

static int is_resolution_supported(int width, int height, double fps) {
    const int MAX_WIDTH = 1920;
    const int MAX_HEIGHT = 1080;
    const double MAX_FPS = 30.0;
    
    if (width > MAX_WIDTH || height > MAX_HEIGHT) {
        fprintf(stderr, "video_decoder: resolution %dx%d exceeds i.MX6ULL limit (%dx%d)\n",
                width, height, MAX_WIDTH, MAX_HEIGHT);
        return 0;
    }
    
    if (fps > MAX_FPS) {
        fprintf(stderr, "video_decoder: fps %.2f exceeds i.MX6ULL limit (%.0ffps)\n", fps, MAX_FPS);
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

    printf("video_decoder: opening %s\n", file_path);
    
    if (avformat_open_input(&vd->fmt_ctx, file_path, NULL, NULL) < 0) {
        fprintf(stderr, "video_decoder: cannot open input\n");
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
    }
}

void video_decoder_stop(VideoDecoder *vd) {
    if (!vd || !vd->running) return;
    vd->running = 0;
    pthread_join(vd->thread, NULL);
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
    free(vd);
}