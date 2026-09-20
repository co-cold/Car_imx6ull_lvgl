#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

/*
 * video_decoder.h — 视频解码器接口
 *
 * 使用 libavcodec/FFmpeg 解码视频帧为 RGB/RGBA，
 * 写入双缓冲区供 UI 渲染。
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "video_double_buf.h"
#include "../utils/debug.h"

/**
 * @brief 视频解码器上下文结构体
 */
typedef struct {
    char *file_path;                        /**< 输入文件路径 */
    struct AVFormatContext *fmt_ctx;        /**< FFmpeg 格式上下文 */
    struct AVCodecContext *codec_ctx;       /**< FFmpeg 编解码器上下文 */
    struct SwsContext *sws_ctx;             /**< 图像缩放/格式转换上下文 */
    int video_stream_idx;                   /**< 视频流索引 */
    int dst_width, dst_height;              /**< 目标显示宽高 */
    VideoDoubleBuf *vdb;                    /**< 双缓冲管理器 */
    pthread_t thread;                       /**< 解码线程 ID */
    volatile int running;                   /**< 线程运行标志 */
    volatile int paused;                    /**< 暂停标志 */
    volatile int seek_req;                  /**< 跳转请求（暂未实现） */
    double duration;                        /**< 媒体总时长（秒） */
    double fps;                             /**< 视频帧率 */
    
    // 内部同步变量，用于线程退出确认
    pthread_mutex_t exit_lock;              /**< 退出同步锁 */
    pthread_cond_t exit_cond;               /**< 退出同步条件变量 */
    volatile int thread_exited;             /**< 线程退出标志 */
} VideoDecoder;

/**
 * @brief 初始化视频解码器
 * @param file_path 视频文件路径
 * @param vdb 视频双缓冲管理器
 * @param dst_width 目标显示宽度
 * @param dst_height 目标显示高度
 * @return 成功返回 VideoDecoder 指针，失败返回 NULL
 */
VideoDecoder* video_decoder_init(const char *file_path, VideoDoubleBuf *vdb,
                                 int dst_width, int dst_height);

/**
 * @brief 启动视频解码线程
 * @param vd 视频解码器实例
 */
void video_decoder_start(VideoDecoder *vd);

/**
 * @brief 停止并等待视频解码线程退出
 * @param vd 视频解码器实例
 */
void video_decoder_stop(VideoDecoder *vd);

/**
 * @brief 请求跳转到指定时间点（当前未实现）
 * @param vd 视频解码器实例
 * @param seconds 目标时间（单位：秒）
 */
void video_decoder_seek(VideoDecoder *vd, double seconds);

/**
 * @brief 暂停视频解码
 * @param vd 视频解码器实例
 */
void video_decoder_pause(VideoDecoder *vd);

/**
 * @brief 恢复视频解码
 * @param vd 视频解码器实例
 */
void video_decoder_resume(VideoDecoder *vd);

/**
 * @brief 释放视频解码器所有资源
 * @param vd 视频解码器实例
 */
void video_decoder_free(VideoDecoder *vd);

#endif