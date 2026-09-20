#ifndef MEDIA_METADATA_H
#define MEDIA_METADATA_H

/*
 * media_metadata.h — 媒体文件元数据解析
 *
 * 读取音频/视频文件的标签信息（标题、艺术家、时长等），
 * 支持格式化和时长转换输出。
 */

#include <stdint.h>
#include <stddef.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
/**
 * @brief 格式化时长为短格式字符串（不显示小时）
 * @param duration_ms 时长（毫秒）
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 */
void media_metadata_format_duration_short(int64_t duration_ms, char *buf, size_t buf_size);

#endif

/**
 * @brief 媒体类型枚举
 */
typedef enum {
    MEDIA_TYPE_UNKNOWN = 0,
    MEDIA_TYPE_AUDIO,     // 音频文件
    MEDIA_TYPE_VIDEO,     // 视频文件
    MEDIA_TYPE_IMAGE      // 图片文件
} MediaType;

/**
 * @brief 媒体元数据结构体（音频/视频通用）
 */
typedef struct {
    // 基础信息
    MediaType type;           // 媒体类型
    char *file_path;          // 文件完整路径
    char *file_name;          // 文件名（含扩展名）
    char *file_ext;           // 文件扩展名
    
    // 音频/视频通用
    int64_t duration_ms;      // 时长（毫秒）
    int bit_rate;             // 比特率（bps）
    
    // 音频特有
    char *audio_title;        // 歌曲标题
    char *audio_artist;       // 歌手/艺术家
    char *audio_album;        // 专辑名称
    int audio_channels;       // 声道数
    int audio_sample_rate;    // 采样率（Hz）
    
    // 视频特有
    char *video_title;        // 视频标题
    int video_width;          // 视频宽度
    int video_height;         // 视频高度
    double video_fps;         // 帧率
    
    // 通用描述
    char *description;        // 描述信息
    
    // 封面图片
    uint8_t *cover_data;      // 封面图片数据（解码后的 RGB565 像素）
    size_t cover_size;        // 封面数据大小（字节）
    int cover_w;              // 封面宽度
    int cover_h;              // 封面高度
} MediaMetadata;

int media_metadata_parse(const char *file_path, MediaMetadata *info);
void media_metadata_free(MediaMetadata *info);
char* media_metadata_format_duration(int64_t duration_ms, char *buf, size_t buf_size);
char* media_metadata_format_duration_short(int64_t duration_ms, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif // MEDIA_METADATA_H