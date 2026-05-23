#include "media_metadata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

// 调试宏控制
#ifndef DEBUG
#define DEBUG 1
char* media_metadata_format_duration_short(int64_t duration_ms, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) return NULL;
    
    int hours = duration_ms / 3600000;
    int minutes = (duration_ms % 3600000) / 60000;
    int seconds = (duration_ms % 60000) / 1000;
    
    if (hours > 0) {
        snprintf(buf, buf_size, "%d:%02d:%02d", hours, minutes, seconds);
    } else {
        snprintf(buf, buf_size, "%d:%02d", minutes, seconds);
    }
    return buf;
}

#endif

#if DEBUG
#define LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define LOGD(fmt, ...)
#define LOGE(fmt, ...)
#endif

int media_metadata_parse(const char *file_path, MediaMetadata *info) {
    if (!file_path || !info) return -1;
    
    // 初始化
    memset(info, 0, sizeof(MediaMetadata));
    
    // 设置文件路径
    info->file_path = strdup(file_path);
    
    // 提取文件名
    char *dup = strdup(file_path);
    info->file_name = strdup(basename(dup));
    free(dup);
    
    // 提取扩展名
    dup = strdup(info->file_name);
    char *dot = strrchr(dup, '.');
    if (dot) {
        info->file_ext = strdup(dot + 1);
        *dot = '\0';
    }
    
    // 使用 FFmpeg 解析元数据
    AVFormatContext *fmt_ctx = NULL;
    int ret = avformat_open_input(&fmt_ctx, file_path, NULL, NULL);
    if (ret < 0) {
        // FFmpeg 解析失败，使用文件名作为标题
        info->type = MEDIA_TYPE_UNKNOWN;
        info->audio_title = strdup(info->file_name);
        info->audio_artist = strdup("未知歌手");
        free(dup);
        return 0;
    }
    
    // 获取流信息
    avformat_find_stream_info(fmt_ctx, NULL);
    
    // 设置时长（转换为毫秒）
    info->duration_ms = fmt_ctx->duration / 1000;
    info->bit_rate = fmt_ctx->bit_rate;
    
    // 判断媒体类型
    // 注意：FLAC/MP3 文件中的封面图片会被识别为视频流，需要区分真正的视频
    int has_audio = 0;
    int has_real_video = 0;
    
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVCodecParameters *codecpar = fmt_ctx->streams[i]->codecpar;
        
        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            has_audio = 1;
            // FFmpeg 6.0+ 使用新版 API
            info->audio_channels = codecpar->ch_layout.nb_channels;
            info->audio_sample_rate = codecpar->sample_rate;
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            // 判断是否为真正的视频（封面图片通常帧率异常或只有1帧）
            double fps = av_q2d(fmt_ctx->streams[i]->r_frame_rate);
            int is_cover_art = 0;
            
            // 封面图片的特征：
            // 1. 帧率异常（非常高如90000fps，或非常低）
            // 2. 持续时间为0
            if (fps > 1000 || fps < 0.1) {
                is_cover_art = 1;
            }
            
            if (!is_cover_art) {
                has_real_video = 1;
                info->video_width = codecpar->width;
                info->video_height = codecpar->height;
                info->video_fps = fps;
            }
        }
    }
    
    // 判断逻辑：
    // 1. 如果有真正的视频流 → 判断为视频
    // 2. 如果只有音频流 → 判断为音频
    // 3. 如果只有封面图片（假视频流）+ 音频 → 判断为音频
    if (has_real_video) {
        info->type = MEDIA_TYPE_VIDEO;
    } else if (has_audio) {
        info->type = MEDIA_TYPE_AUDIO;
    } else {
        info->type = MEDIA_TYPE_UNKNOWN;
    }
    
    // 读取元数据
    AVDictionaryEntry *tag = NULL;
    
    // 尝试读取标题（支持多种标签名）
    tag = av_dict_get(fmt_ctx->metadata, "title", NULL, 0);
    if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "TITLE", NULL, 0);
    if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "Title", NULL, 0);
    if (tag && tag->value) {
        if (info->type == MEDIA_TYPE_VIDEO) {
            info->video_title = strdup(tag->value);
        } else {
            info->audio_title = strdup(tag->value);
        }
    } else {
        // 使用文件名作为标题
        if (info->type == MEDIA_TYPE_VIDEO) {
            info->video_title = strdup(info->file_name);
        } else {
            info->audio_title = strdup(info->file_name);
        }
    }
    
    // 音频特有元数据
    if (info->type == MEDIA_TYPE_AUDIO) {
        // 读取艺术家（支持多种标签名）
        tag = av_dict_get(fmt_ctx->metadata, "artist", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "ARTIST", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "Artist", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "artists", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "ARTISTS", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "ALBUMARTIST", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "albumartist", NULL, 0);
        if (tag && tag->value) {
            info->audio_artist = strdup(tag->value);
        } else {
            info->audio_artist = strdup("未知歌手");
        }
        
        // 读取专辑（支持多种标签名）
        tag = av_dict_get(fmt_ctx->metadata, "album", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "ALBUM", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "Album", NULL, 0);
        if (tag && tag->value) {
            info->audio_album = strdup(tag->value);
        }
    }
    
    // 读取封面图片（FLAC/MP3等格式支持）
    // 方法1：尝试从视频流读取（FLAC常用方式）
    if (!info->cover_data) {
        for (int i = 0; i < fmt_ctx->nb_streams; i++) {
            AVCodecParameters *codecpar = fmt_ctx->streams[i]->codecpar;
            if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                // 判断是否为封面图片（帧率异常或持续时间为0）
                double fps = av_q2d(fmt_ctx->streams[i]->r_frame_rate);
                if (fps > 1000 || fps < 0.1) {
                    AVPacket pkt;
                    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
                        if (pkt.stream_index == i) {
                            info->cover_data = malloc(pkt.size);
                            if (info->cover_data) {
                                memcpy(info->cover_data, pkt.data, pkt.size);
                                info->cover_size = pkt.size;
                                LOGD("media_metadata: cover found in video stream, size=%zu\n", pkt.size);
                            }
                            break;
                        }
                        av_packet_unref(&pkt);
                    }
                    break;
                }
            }
        }
    }
    
    // 方法2：尝试从 attachment 流读取
    if (!info->cover_data) {
        for (int i = 0; i < fmt_ctx->nb_streams; i++) {
            AVCodecParameters *codecpar = fmt_ctx->streams[i]->codecpar;
            if (codecpar->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
                AVPacket pkt;
                while (av_read_frame(fmt_ctx, &pkt) >= 0) {
                    if (pkt.stream_index == i) {
                        info->cover_data = malloc(pkt.size);
                        if (info->cover_data) {
                            memcpy(info->cover_data, pkt.data, pkt.size);
                            info->cover_size = pkt.size;
                            LOGD("media_metadata: cover found in attachment stream, size=%zu\n", pkt.size);
                        }
                    }
                    av_packet_unref(&pkt);
                }
                break;
            }
        }
    }
    
    // 方法3：如果以上都没有找到，尝试从 metadata 的 cover 标签读取
    if (!info->cover_data) {
        tag = av_dict_get(fmt_ctx->metadata, "cover", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "COVER", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "COVER ART", NULL, 0);
        if (!tag || !tag->value) tag = av_dict_get(fmt_ctx->metadata, "cover art", NULL, 0);
        if (tag && tag->value) {
            // cover 标签可能包含 base64 编码的图片数据
            // 这里简单处理，直接存储（实际应用中需要解码 base64）
            size_t len = strlen(tag->value);
            info->cover_data = malloc(len + 1);
            if (info->cover_data) {
                memcpy(info->cover_data, tag->value, len + 1);
                info->cover_size = len;
                LOGD("media_metadata: cover found in metadata, size=%zu\n", len);
            }
        }
    }
    
    free(dup);
    avformat_close_input(&fmt_ctx);
    return 0;
}

void media_metadata_free(MediaMetadata *info) {
    if (!info) return;
    if (info->file_path) free(info->file_path);
    if (info->file_name) free(info->file_name);
    if (info->file_ext) free(info->file_ext);
    if (info->audio_title) free(info->audio_title);
    if (info->audio_artist) free(info->audio_artist);
    if (info->audio_album) free(info->audio_album);
    if (info->video_title) free(info->video_title);
    if (info->description) free(info->description);
    if (info->cover_data) free(info->cover_data);
    memset(info, 0, sizeof(MediaMetadata));
}

char* media_metadata_format_duration(int64_t duration_ms, char *buf, size_t buf_size) {
    int64_t total_seconds = duration_ms / 1000;
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;
    
    if (hours > 0) {
        snprintf(buf, buf_size, "%02d:%02d:%02d", hours, minutes, seconds);
    } else {
        snprintf(buf, buf_size, "%02d:%02d", minutes, seconds);
    }
    return buf;
}