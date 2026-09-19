#ifndef PLAYER_CORE_H
#define PLAYER_CORE_H

#include <stdint.h>
#include "media/playlist.h"

/**
 * @brief 播放器核心控制结构体（不透明）
 */
typedef struct PlayerCore PlayerCore;



/**
 * @brief 创建播放器核心实例
 * @return 成功返回 PlayerCore 指针，失败返回 NULL
 */
PlayerCore* player_core_create(void);

/**
 * @brief 销毁播放器核心实例，释放所有资源
 * @param pc 播放器核心实例
 */
void player_core_destroy(PlayerCore *pc);

/**
 * @brief 播放音频文件
 * @param pc 播放器核心实例
 * @param file_path 音频文件路径
 * @return 成功返回 0，失败返回 -1
 */
int player_core_play_audio(PlayerCore *pc, const char *file_path);

/**
 * @brief 播放视频文件（音频轨道将被忽略）
 * @param pc 播放器核心实例
 * @param file_path 视频文件路径
 * @param dst_width 目标显示宽度
 * @param dst_height 目标显示高度
 * @return 成功返回 0，失败返回 -1
 */
int player_core_play_video(PlayerCore *pc, const char *file_path,
                           int dst_width, int dst_height);

/**
 * @brief 暂停当前播放
 * @param pc 播放器核心实例
 */
void player_core_pause(PlayerCore *pc);

/**
 * @brief 恢复暂停的播放
 * @param pc 播放器核心实例
 */
void player_core_resume(PlayerCore *pc);

/**
 * @brief 停止当前播放，释放媒体资源
 * @param pc 播放器核心实例
 */
void player_core_stop(PlayerCore *pc);

/**
 * @brief 跳转到指定时间位置
 * @param pc 播放器核心实例
 * @param seconds 目标时间（秒）
 */
void player_core_seek(PlayerCore *pc, double seconds);

/**
 * @brief 获取当前播放位置
 * @param pc 播放器核心实例
 * @return 当前时间（秒），失败返回 0.0
 */
double player_core_get_position(PlayerCore *pc);

/**
 * @brief 获取媒体总时长
 * @param pc 播放器核心实例
 * @return 总时长（秒），失败返回 0.0
 */
double player_core_get_duration(PlayerCore *pc);

/**
 * @brief 获取当前播放状态
 * @param pc 播放器核心实例
 * @return 0:idle, 1:playing, 2:paused
 */
int player_core_get_state(PlayerCore *pc);

/**
 * @brief 获取当前就绪的视频帧数据（RGB565格式）
 * @param pc 播放器核心实例
 * @return 视频帧缓冲区指针，若无则返回 NULL
 */
uint8_t* player_core_get_video_frame(PlayerCore *pc);

/**
 * @brief 获取与音频时间匹配的视频帧（用于音画同步）
 * @param pc         播放器核心实例
 * @param audio_time 当前音频播放时间（秒）
 * @return 视频帧缓冲区指针，若当前帧 PTS 超前于音频则返回 NULL
 */
uint8_t* player_core_get_video_frame_at_time(PlayerCore *pc, double audio_time);

/**
 * @brief 释放已获取的视频帧，允许解码器写入新帧
 * @param pc 播放器核心实例
 */
void player_core_release_video_frame(PlayerCore *pc);

// ========== 播放列表管理接口 ==========

/**
 * @brief 设置音频播放列表
 * @param pc 播放器核心实例
 * @param playlist 播放列表指针
 */
void player_core_set_audio_playlist(PlayerCore *pc, Playlist *playlist);

/**
 * @brief 设置视频播放列表
 * @param pc 播放器核心实例
 * @param playlist 播放列表指针
 */
void player_core_set_video_playlist(PlayerCore *pc, Playlist *playlist);

/**
 * @brief 获取音频播放列表
 * @param pc 播放器核心实例
 * @return 播放列表指针
 */
Playlist* player_core_get_audio_playlist(PlayerCore *pc);

/**
 * @brief 获取视频播放列表
 * @param pc 播放器核心实例
 * @return 播放列表指针
 */
Playlist* player_core_get_video_playlist(PlayerCore *pc);

/**
 * @brief 播放音频列表中的指定歌曲
 * @param pc 播放器核心实例
 * @param idx 歌曲索引
 * @return 成功返回0，失败返回-1
 */
int player_core_play_audio_by_index(PlayerCore *pc, int idx);

/**
 * @brief 播放视频列表中的指定视频
 * @param pc 播放器核心实例
 * @param idx 视频索引
 * @param width 目标宽度
 * @param height 目标高度
 * @return 成功返回0，失败返回-1
 */
int player_core_play_video_by_index(PlayerCore *pc, int idx, int width, int height);

/**
 * @brief 播放下一首/下一个
 * @param pc 播放器核心实例
 * @return 成功返回新索引，失败返回-1
 */
int player_core_play_next(PlayerCore *pc);

/**
 * @brief 播放上一首/上一个
 * @param pc 播放器核心实例
 * @return 成功返回新索引，失败返回-1
 */
int player_core_play_prev(PlayerCore *pc);

#endif