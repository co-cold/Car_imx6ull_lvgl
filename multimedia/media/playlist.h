
#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 播放列表类型
typedef enum {
    PLAYLIST_TYPE_AUDIO,
    PLAYLIST_TYPE_VIDEO
} PlaylistType;

// 媒体条目
typedef struct {
    char *file_path;      // 文件路径
    char *title;          // 标题
    char *artist;         // 艺术家（音频）
    char *album;          // 专辑（音频）
    int64_t duration_ms;  // 时长（毫秒）
    uint8_t *cover_data;  // 封面数据（音频）
    size_t cover_size;    // 封面大小
} PlaylistItem;

// 播放列表
typedef struct {
    PlaylistType type;
    PlaylistItem *items;
    int count;
    int capacity;
    int current_idx;
} Playlist;

/**
 * @brief 创建播放列表
 * @param type 播放列表类型
 * @param capacity 容量
 * @return 播放列表指针
 */
Playlist *playlist_create(PlaylistType type, int capacity);

/**
 * @brief 销毁播放列表
 * @param pl 播放列表指针
 */
void playlist_destroy(Playlist *pl);

/**
 * @brief 添加条目
 * @param pl 播放列表指针
 * @param file_path 文件路径
 * @return 成功返回索引，失败返回-1
 */
int playlist_add(Playlist *pl, const char *file_path);

/**
 * @brief 删除条目
 * @param pl 播放列表指针
 * @param idx 索引
 * @return 成功返回0，失败返回-1
 */
int playlist_remove(Playlist *pl, int idx);

/**
 * @brief 更新条目
 * @param pl 播放列表指针
 * @param idx 索引
 * @param file_path 新文件路径
 * @return 成功返回0，失败返回-1
 */
int playlist_update(Playlist *pl, int idx, const char *file_path);

/**
 * @brief 清空播放列表
 * @param pl 播放列表指针
 */
void playlist_clear(Playlist *pl);

/**
 * @brief 交换两个条目
 * @param pl 播放列表指针
 * @param idx1 索引1
 * @param idx2 索引2
 * @return 成功返回0，失败返回-1
 */
int playlist_swap(Playlist *pl, int idx1, int idx2);

/**
 * @brief 获取条目数
 * @param pl 播放列表指针
 * @return 条目数
 */
int playlist_get_count(Playlist *pl);

/**
 * @brief 获取当前索引
 * @param pl 播放列表指针
 * @return 当前索引
 */
int playlist_get_current_idx(Playlist *pl);

/**
 * @brief 设置当前索引
 * @param pl 播放列表指针
 * @param idx 索引
 * @return 成功返回0，失败返回-1
 */
int playlist_set_current_idx(Playlist *pl, int idx);

/**
 * @brief 获取指定索引的条目
 * @param pl 播放列表指针
 * @param idx 索引
 * @return 条目指针，失败返回NULL
 */
PlaylistItem *playlist_get_item(Playlist *pl, int idx);

/**
 * @brief 获取当前条目
 * @param pl 播放列表指针
 * @return 当前条目指针，失败返回NULL
 */
PlaylistItem *playlist_get_current_item(Playlist *pl);

/**
 * @brief 播放下一个
 * @param pl 播放列表指针
 * @return 成功返回下一个索引，失败返回-1
 */
int playlist_next(Playlist *pl);

/**
 * @brief 播放上一个
 * @param pl 播放列表指针
 * @return 成功返回上一个索引，失败返回-1
 */
int playlist_prev(Playlist *pl);

/**
 * @brief 扫描目录并添加媒体文件
 * @param pl 播放列表指针
 * @param dir_path 目录路径
 * @return 添加的文件数量
 */
int playlist_scan_dir(Playlist *pl, const char *dir_path);

#ifdef __cplusplus
}
#endif

#endif // PLAYLIST_H