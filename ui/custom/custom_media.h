#ifndef CUSTOM_MEDIA_H
#define CUSTOM_MEDIA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "gui_guider.h"
#include "media/playlist.h"

// 调试宏控制
#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#define LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOGD(fmt, ...)
#endif

// 配置常量
#define MAX_SONGS 50
#define MAX_VIDEOS 20
#define MAX_LYRICS_LINES 200
#define LYRICS_DISPLAY_LINES 5
#define LYRICS_FONT_SIZE_NORMAL 16
#define LYRICS_FONT_SIZE_HIGHLI 24
#define VIDEO_WIDTH 800
#define VIDEO_HEIGHT 450

#include "ipc/lvgl_dbus_protocol.h"

// 媒体播放上下文结构体
typedef struct {
    lv_ui *ui;
    lv_timer_t *progress_timer;
    lv_timer_t *video_frame_timer;
    Playlist *audio_playlist;
    Playlist *video_playlist;
    int play_mode;
    char *lyrics[MAX_LYRICS_LINES];
    double lyrics_time[MAX_LYRICS_LINES];
    int lyrics_count;
    int current_lyric_idx;
    bool playback_just_finished;
    double last_pos;
    volatile bool is_deinitializing;
} MediaContext;

// 获取全局上下文
MediaContext* custom_media_get_context(void);

// 前向声明

void custom_media_init(lv_ui *ui);
void custom_media_cleanup_screen(void);
void custom_media_deinit(void);

int custom_media_play_audio(const char *file);
int custom_media_play_video(const char *file, int width, int height);
void custom_media_pause(void);
void custom_media_resume(void);
void custom_media_stop(void);
void custom_media_seek(double seconds);

double custom_media_get_position(void);
double custom_media_get_duration(void);

/**
 * @brief 获取当前播放状态
 * @return 0:idle, 1:playing, 2:paused
 */
int custom_media_get_state(void);

/**
 * @brief 设置音量
 * @param volume 音量值（0.0 - 2.0，1.0为原始音量）
 */
void custom_media_set_volume(float volume);

/**
 * @brief 获取当前音量
 * @return 当前音量值
 */
float custom_media_get_volume(void);

// 视频帧获取（LVGL定时器中使用）
uint8_t* custom_media_get_video_frame(void);
void custom_media_release_video_frame(void);

/**
 * @brief 更新视频帧显示（供 LVGL 定时器调用）
 * @param timer LVGL 定时器指针
 */
void custom_media_update_video_frame(lv_timer_t *timer);

/**
 * @brief 更新视频进度条
 */
void custom_media_update_video_progress(void);

/**
 * @brief 更新视频时间显示
 */
void custom_media_update_video_time_display(void);

/**
 * @brief 更新视频文件名显示
 * @param file_name 文件名
 */
void custom_media_update_video_filename(const char *file_name);

// 播放列表管理
int custom_media_play_song_by_index(int idx);
int custom_media_play_next(void);
int custom_media_play_prev(void);
int custom_media_get_current_song_idx(void);
int custom_media_get_song_count(void);
const char* custom_media_get_song_path(int idx);
char* custom_media_get_song_name(const char *path);

// 视频播放列表管理
int custom_media_play_video_by_index(int idx);
int custom_media_play_next_video(void);
int custom_media_play_prev_video(void);
int custom_media_get_current_video_idx(void);
int custom_media_get_video_count(void);
const char* custom_media_get_video_path(int idx);

// 歌曲元数据管理
const char* custom_media_get_file_name(int idx);
const char* custom_media_get_song_title(int idx);
const char* custom_media_get_song_artist(int idx);
const char* custom_media_get_song_album(int idx);
int64_t custom_media_get_song_duration_ms(int idx);
void custom_media_format_duration(int64_t duration_ms, char *buf, size_t buf_size);

void custom_media_update_ui_display(void);

// 目录扫描
int custom_media_scan_music_dir(const char *dir_path);
int custom_media_scan_video_dir(const char *dir_path);

// 歌词管理
int custom_media_parse_lyrics(const char *file_path);
const char* custom_media_get_current_lyric(void);
void custom_media_update_lyric_index(double position);

// 进度更新控制
void custom_media_start_progress_update(void);
void custom_media_stop_progress_update(void);

// 获取播放列表指针（供UI更新使用）
Playlist* custom_media_get_audio_playlist(void);
Playlist* custom_media_get_video_playlist(void);

// ========== 模块初始化函数 ==========

void custom_media_init_music(void);
void custom_media_init_video(void);

int custom_media_get_play_mode(void);

// ========== 列表 UI 初始化函数 ==========

/**
 * @brief 初始化音乐列表 UI（清空原有项，加载播放列表）
 * @param ui LVGL UI 指针
 */
void custom_media_init_music_list(lv_ui *ui);

/**
 * @brief 初始化视频列表 UI（清空原有项，加载播放列表）
 * @param ui LVGL UI 指针
 */
void custom_media_init_video_list(lv_ui *ui);

#endif