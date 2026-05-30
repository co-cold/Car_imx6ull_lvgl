
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "playlist.h"
#include "media_metadata.h"
#include "../utils/debug.h"

// 支持的音频格式
static const char *audio_extensions[] = {".mp3", ".flac", ".wav", ".ogg", ".m4a", NULL};

// 支持的视频格式
static const char *video_extensions[] = {".avi", ".mp4", ".mkv", ".flv", ".mov", ".wmv", ".mpeg", ".mpg", ".m4v", NULL};

static int is_audio_file(const char *filename) {
    if (!filename) return 0;
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    for (int i = 0; audio_extensions[i]; i++) {
        if (strcasecmp(ext, audio_extensions[i]) == 0) return 1;
    }
    return 0;
}

static int is_video_file(const char *filename) {
    if (!filename) return 0;
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    for (int i = 0; video_extensions[i]; i++) {
        if (strcasecmp(ext, video_extensions[i]) == 0) return 1;
    }
    return 0;
}

static void playlist_item_free(PlaylistItem *item) {
    if (!item) return;
    if (item->file_path) free(item->file_path);
    if (item->title) free(item->title);
    if (item->artist) free(item->artist);
    if (item->album) free(item->album);
    if (item->cover_data) free(item->cover_data);
    memset(item, 0, sizeof(PlaylistItem));
}

Playlist *playlist_create(PlaylistType type, int capacity) {
    if (capacity <= 0) return NULL;
    
    Playlist *pl = malloc(sizeof(Playlist));
    if (!pl) return NULL;
    
    pl->type = type;
    pl->capacity = capacity;
    pl->count = 0;
    pl->current_idx = 0;
    pl->items = calloc(capacity, sizeof(PlaylistItem));
    if (!pl->items) {
        free(pl);
        return NULL;
    }
    
    LOGD("created, type=%d, capacity=%d\n", type, capacity);
    return pl;
}

void playlist_destroy(Playlist *pl) {
    if (!pl) return;
    playlist_clear(pl);
    free(pl->items);
    free(pl);
    LOGD("destroyed\n");
}

int playlist_add(Playlist *pl, const char *file_path) {
    if (!pl || !file_path) return -1;
    if (pl->count >= pl->capacity) return -1;
    
    // 检查类型匹配
    int is_valid = (pl->type == PLAYLIST_TYPE_AUDIO) ? is_audio_file(file_path) : is_video_file(file_path);
    if (!is_valid) return -1;
    
    // 检查是否已存在
    for (int i = 0; i < pl->count; i++) {
        if (strcmp(pl->items[i].file_path, file_path) == 0) {
            return i;
        }
    }
    
    PlaylistItem *item = &pl->items[pl->count];
    item->file_path = strdup(file_path);
    
    // 解析元数据（仅音频）
    if (pl->type == PLAYLIST_TYPE_AUDIO) {
        MediaMetadata meta;
        memset(&meta, 0, sizeof(MediaMetadata));
        media_metadata_parse(file_path, &meta);
        
        if (meta.audio_title) item->title = strdup(meta.audio_title);
        if (meta.audio_artist) item->artist = strdup(meta.audio_artist);
        if (meta.audio_album) item->album = strdup(meta.audio_album);
        item->duration_ms = meta.duration_ms;
        
        if (meta.cover_data && meta.cover_size > 0) {
            item->cover_data = malloc(meta.cover_size);
            memcpy(item->cover_data, meta.cover_data, meta.cover_size);
            item->cover_size = meta.cover_size;
        }
        
        media_metadata_free(&meta);
    } else {
        // 视频：提取文件名作为标题
        const char *slash = strrchr(file_path, '/');
        const char *name = slash ? slash + 1 : file_path;
        item->title = strdup(name);
    }
    
    LOGD("added %s at index %d\n", file_path, pl->count);
    return pl->count++;
}

int playlist_remove(Playlist *pl, int idx) {
    if (!pl || idx < 0 || idx >= pl->count) return -1;
    
    playlist_item_free(&pl->items[idx]);
    
    for (int i = idx; i < pl->count - 1; i++) {
        pl->items[i] = pl->items[i + 1];
        memset(&pl->items[i + 1], 0, sizeof(PlaylistItem));
    }
    
    pl->count--;
    
    if (pl->current_idx >= pl->count) {
        pl->current_idx = pl->count > 0 ? pl->count - 1 : 0;
    }
    
    LOGD("removed index %d, count=%d\n", idx, pl->count);
    return 0;
}

int playlist_update(Playlist *pl, int idx, const char *file_path) {
    if (!pl || !file_path || idx < 0 || idx >= pl->count) return -1;
    
    int is_valid = (pl->type == PLAYLIST_TYPE_AUDIO) ? is_audio_file(file_path) : is_video_file(file_path);
    if (!is_valid) return -1;
    
    // 释放旧数据
    playlist_item_free(&pl->items[idx]);
    
    // 设置新路径
    pl->items[idx].file_path = strdup(file_path);
    
    // 重新解析元数据
    if (pl->type == PLAYLIST_TYPE_AUDIO) {
        MediaMetadata meta;
        memset(&meta, 0, sizeof(MediaMetadata));
        media_metadata_parse(file_path, &meta);
        
        if (meta.audio_title) pl->items[idx].title = strdup(meta.audio_title);
        if (meta.audio_artist) pl->items[idx].artist = strdup(meta.audio_artist);
        if (meta.audio_album) pl->items[idx].album = strdup(meta.audio_album);
        pl->items[idx].duration_ms = meta.duration_ms;
        
        if (meta.cover_data && meta.cover_size > 0) {
            pl->items[idx].cover_data = malloc(meta.cover_size);
            memcpy(pl->items[idx].cover_data, meta.cover_data, meta.cover_size);
            pl->items[idx].cover_size = meta.cover_size;
        }
        
        media_metadata_free(&meta);
    } else {
        const char *slash = strrchr(file_path, '/');
        const char *name = slash ? slash + 1 : file_path;
        pl->items[idx].title = strdup(name);
    }
    
    LOGD("updated index %d to %s\n", idx, file_path);
    return 0;
}

void playlist_clear(Playlist *pl) {
    if (!pl) return;
    for (int i = 0; i < pl->count; i++) {
        playlist_item_free(&pl->items[i]);
    }
    pl->count = 0;
    pl->current_idx = 0;
    LOGD("cleared\n");
}

int playlist_swap(Playlist *pl, int idx1, int idx2) {
    if (!pl || idx1 < 0 || idx1 >= pl->count || idx2 < 0 || idx2 >= pl->count) {
        return -1;
    }
    if (idx1 == idx2) return 0;
    
    PlaylistItem tmp = pl->items[idx1];
    pl->items[idx1] = pl->items[idx2];
    pl->items[idx2] = tmp;
    
    if (pl->current_idx == idx1) {
        pl->current_idx = idx2;
    } else if (pl->current_idx == idx2) {
        pl->current_idx = idx1;
    }
    
    LOGD("swapped %d and %d\n", idx1, idx2);
    return 0;
}

int playlist_get_count(Playlist *pl) {
    return pl ? pl->count : 0;
}

int playlist_get_current_idx(Playlist *pl) {
    return pl ? pl->current_idx : -1;
}

int playlist_set_current_idx(Playlist *pl, int idx) {
    if (!pl || idx < 0 || idx >= pl->count) return -1;
    pl->current_idx = idx;
    return 0;
}

PlaylistItem *playlist_get_item(Playlist *pl, int idx) {
    if (!pl || idx < 0 || idx >= pl->count) return NULL;
    return &pl->items[idx];
}

PlaylistItem *playlist_get_current_item(Playlist *pl) {
    if (!pl || pl->count == 0) return NULL;
    return &pl->items[pl->current_idx];
}

int playlist_next(Playlist *pl) {
    if (!pl || pl->count <= 0) return -1;
    pl->current_idx = (pl->current_idx + 1) % pl->count;
    return pl->current_idx;
}

int playlist_prev(Playlist *pl) {
    if (!pl || pl->count <= 0) return -1;
    pl->current_idx = (pl->current_idx - 1 + pl->count) % pl->count;
    return pl->current_idx;
}

int playlist_scan_dir(Playlist *pl, const char *dir_path) {
    if (!pl || !dir_path) return -1;
    
    DIR *dir = opendir(dir_path);
    if (!dir) {
        LOGE("failed to open directory %s\n", dir_path);
        return -1;
    }
    
    int added = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;
        
        int is_valid = (pl->type == PLAYLIST_TYPE_AUDIO) ? 
            is_audio_file(entry->d_name) : is_video_file(entry->d_name);
        
        if (is_valid) {
            char *full_path = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
            sprintf(full_path, "%s/%s", dir_path, entry->d_name);
            
            if (playlist_add(pl, full_path) >= 0) {
                added++;
            }
            free(full_path);
        }
    }
    
    closedir(dir);
    LOGD("scanned %d files from %s\n", added, dir_path);
    return added;
}