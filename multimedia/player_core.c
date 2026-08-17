#include "player_core.h"
#include "audio/audio_decoder.h"
#include "audio/audio_output.h"
#include "video/video_decoder.h"
#include "video/video_double_buf.h"
#include "utils/ring_buffer.h"
#include "media/playlist.h"
#include "utils/debug.h"
#include <stdlib.h>
#include <stdio.h>

struct PlayerCore {
    // 共享环形缓冲区
    RingBuffer *rb;
    AudioOutput *ao;
    AudioDecoder *ad;

    VideoDecoder *vd;
    VideoDoubleBuf *vdb;

    int state; // 0 idle, 1 playing, 2 paused
    pthread_mutex_t lock;
    int video_mode; // 0 audio only, 1 video
    int sample_rate; // 当前采样率（0表示未设置）
    float volume; // 当前音量值（0.0-1.0）
    
    // 视频分辨率
    int video_width;
    int video_height;
    
    // 播放列表
    Playlist *audio_playlist;
    Playlist *video_playlist;
};

PlayerCore* player_core_create(void) {
    PlayerCore *pc = calloc(1, sizeof(PlayerCore));
    if (!pc) return NULL;

    // 环形缓冲区：16 * period_size * 4字节（16bit stereo），增大缓冲区避免死锁
    pc->rb = rb_init(1024 * 16 * 4);
    if (!pc->rb) {
        free(pc);
        return NULL;
    }

    // 初始采样率设为 0，表示尚未设置（延迟初始化音频输出）
    pc->sample_rate = 0;
    
    // 初始化默认音量（与进度条默认值50%同步）
    pc->volume = 0.5f;
    
    pthread_mutex_init(&pc->lock, NULL);
    return pc;
}

void player_core_destroy(PlayerCore *pc) {
    LOGD("player_core_destroy: 开始销毁播放器核心\n");
    if (!pc) {
        LOGD("player_core_destroy: pc为NULL，直接返回\n");
        return;
    }
    
    // 确保所有组件都已停止，避免在销毁环形缓冲区时仍有组件在使用它
    player_core_stop(pc);
    
    // 释放共享的环形缓冲区
    LOGD("player_core_destroy: 释放环形缓冲区\n");
    if (pc->rb) {
        LOGD("player_core_destroy: 调用rb_free\n");
        rb_free(pc->rb);
        LOGD("player_core_destroy: rb_free 完成\n");
        pc->rb = NULL;
    } else {
        LOGD("player_core_destroy: pc->rb 为 NULL\n");
    }
    
    // 最后释放播放器核心自身的锁
    LOGD("player_core_destroy: 销毁播放器核心互斥锁\n");
    pthread_mutex_destroy(&pc->lock);
    LOGD("player_core_destroy: 释放播放器核心内存\n");
    free(pc);
    LOGD("player_core_destroy: 完成销毁播放器核心\n");
}

int player_core_play_audio(PlayerCore *pc, const char *file_path) {
    if (!pc) return -1;
    
    // 无论当前是什么状态，都完全停止并释放所有资源
    player_core_stop(pc);
    
    // 等待资源完全释放
    usleep(100000);  // 等待 100ms
    
    // 重置环形缓冲区
    rb_reset(pc->rb);

    pc->ad = audio_decoder_init(file_path, pc->rb);
    if (!pc->ad) return -1;

    // 使用文件的原生采样率和格式创建音频输出
    pc->sample_rate = pc->ad->sample_rate;
    pc->ao = audio_output_init(pc->sample_rate, pc->ad->channels, 2048, pc->rb);
    if (!pc->ao) {
        audio_decoder_free(pc->ad);
        pc->ad = NULL;
        return -1;
    }
    
    // 设置音量
    audio_output_set_volume_all(pc->ao, pc->volume);

    audio_decoder_start(pc->ad);
    audio_output_start(pc->ao);
    audio_output_resume(pc->ao);
    pc->state = 1;
    pc->video_mode = 0;
    return 0;
}

int player_core_play_video(PlayerCore *pc, const char *file_path,
                           int dst_width, int dst_height) {
    if (!pc) return -1;
    player_core_stop(pc);

    // 保存分辨率
    pc->video_width = dst_width;
    pc->video_height = dst_height;

    LOGD("playing video %s, %dx%d\n", file_path, dst_width, dst_height);
    
    // 创建视频双缓冲
    int buf_size = dst_width * dst_height * 2; // RGB565
    pc->vdb = vdb_create(buf_size);
    if (!pc->vdb) return -1;

    pc->vd = video_decoder_init(file_path, pc->vdb, dst_width, dst_height);
    if (!pc->vd) {
        vdb_destroy(pc->vdb);
        pc->vdb = NULL;
        return -1;
    }

    // 先启动视频解码器
    video_decoder_start(pc->vd);
    
    // 等待视频解码器解码出第一帧（最多等待500ms），不释放，留给后续显示
    int wait_count = 0;
    while (wait_count < 50) {
        uint8_t *buf = vdb_get_ready_buf(pc->vdb);
        if (buf) {
            break;
        }
        usleep(10000); // 等待10ms
        wait_count++;
    }
    LOGD("video ready after %d ms\n", wait_count * 10);

    // 重置环形缓冲区，清除上一个视频的残留音频数据
    rb_reset(pc->rb);

    // 初始化音频解码器和输出
    pc->ad = audio_decoder_init(file_path, pc->rb);
    if (pc->ad) {
        // 使用文件的原生采样率创建音频输出
        pc->sample_rate = pc->ad->sample_rate;
        pc->ao = audio_output_init(pc->sample_rate, pc->ad->channels, 2048, pc->rb);
        if (pc->ao) {
            // 恢复之前设置的音量（避免切换歌曲后音量丢失）
            audio_output_set_volume_all(pc->ao, pc->volume);
            
            audio_decoder_start(pc->ad);
            audio_output_start(pc->ao);
            audio_output_resume(pc->ao);
        } else {
            audio_decoder_free(pc->ad);
            pc->ad = NULL;
        }
    }

    pc->state = 1;
    pc->video_mode = 1;
    return 0;
}

void player_core_pause(PlayerCore *pc) {
    if (!pc || pc->state != 1) return;
    if (pc->video_mode) {
        // 视频暂停：停止音频输出和视频解码
        if (pc->ao) {
            audio_output_pause(pc->ao);
        }
        // 设置视频解码器暂停标志
        if (pc->vd) {
            video_decoder_pause(pc->vd);
        }
    } else {
        audio_output_pause(pc->ao);
    }
    pc->state = 2;
}

void player_core_resume(PlayerCore *pc) {
    if (!pc || pc->state != 2) return;
    if (pc->video_mode) {
        // 视频恢复：恢复音频输出和视频解码
        if (pc->vd) {
            video_decoder_resume(pc->vd);
        }
        if (pc->ao) {
            audio_output_resume(pc->ao);
        }
    } else {
        audio_output_resume(pc->ao);
    }
    pc->state = 1;
}

void player_core_stop(PlayerCore *pc) {
    LOGD("player_core_stop: 开始停止，state=%d, video_mode=%d\n", pc ? pc->state : -1, pc ? pc->video_mode : -1);
    if (!pc) {
        LOGD("player_core_stop: pc为NULL，直接返回\n");
        return;
    }
    
    // 首先设置状态为停止，避免其他地方继续操作
    LOGD("player_core_stop: 设置状态为停止\n");
    pc->state = 0;
    
    // 先停止音频解码器
    if (pc->ad) {
        LOGD("player_core_stop: 停止音频解码器\n");
        audio_decoder_stop(pc->ad);
        LOGD("player_core_stop: 释放音频解码器\n");
        audio_decoder_free(pc->ad);
        pc->ad = NULL;
        LOGD("player_core_stop: 音频解码器已设置为NULL\n");
    } else {
        LOGD("player_core_stop: 音频解码器为NULL，跳过\n");
    }
    
    // 然后停止音频输出
    if (pc->ao) {
        LOGD("player_core_stop: 停止音频输出\n");
        audio_output_stop(pc->ao);
        LOGD("player_core_stop: 释放音频输出\n");
        audio_output_free(pc->ao);
        pc->ao = NULL;
        LOGD("player_core_stop: 音频输出已设置为NULL\n");
    } else {
        LOGD("player_core_stop: 音频输出为NULL，跳过\n");
    }
    
    // 停止视频解码器
    if (pc->vd) {
        LOGD("player_core_stop: 停止视频解码器\n");
        video_decoder_stop(pc->vd);
        // 等待视频解码器线程完全退出
        LOGD("player_core_stop: 等待视频解码器线程退出\n");
        usleep(50000);  // 50ms
        LOGD("player_core_stop: 释放视频解码器\n");
        video_decoder_free(pc->vd);
        pc->vd = NULL;
        LOGD("player_core_stop: 视频解码器已设置为NULL\n");
    } else {
        LOGD("player_core_stop: 视频解码器为NULL，跳过\n");
    }
    
    // 销毁视频双缓冲
    if (pc->vdb) {
        LOGD("player_core_stop: 销毁视频双缓冲\n");
        vdb_destroy(pc->vdb);
        pc->vdb = NULL;
        LOGD("player_core_stop: 视频双缓冲已设置为NULL\n");
    } else {
        LOGD("player_core_stop: 视频双缓冲为NULL，跳过\n");
    }
    
    // 重置采样率，确保下次播放时重新初始化音频输出
    LOGD("player_core_stop: 重置采样率和视频模式\n");
    pc->sample_rate = 0;
    pc->video_mode = 0;
    LOGD("player_core_stop: 停止完成\n");
}

void player_core_seek(PlayerCore *pc, double seconds) {
    if (!pc) return;
    if (pc->ad) audio_decoder_seek(pc->ad, seconds);
    if (pc->vd) video_decoder_seek(pc->vd, seconds);
}

double player_core_get_position(PlayerCore *pc) {
    if (!pc || !pc->ad) return 0.0;
    return pc->ad->current_time;
}

double player_core_get_duration(PlayerCore *pc) {
    if (!pc) return 0.0;
    if (pc->ad) return pc->ad->duration;
    if (pc->vd) return pc->vd->duration;
    return 0.0;
}

/**
 * @brief 设置音量
 * @param pc      PlayerCore 实例
 * @param volume  音量值（0.0 - 2.0，1.0为原始音量）
 */
void player_core_set_volume(PlayerCore *pc, float volume) {
    if (!pc) return;
    
    // 保存音量值（用于切换歌曲后恢复）
    pc->volume = volume;
    
    // 使用 audio_output_set_volume_all 设置所有控件（针对 WM8960）
    if (pc->ao) {
        audio_output_set_volume_all(pc->ao, volume);
    }
}

/**
 * @brief 获取当前音量
 * @param pc  PlayerCore 实例
 * @return 当前音量值
 */
float player_core_get_volume(PlayerCore *pc) {
    return pc && pc->ao ? audio_output_get_volume(pc->ao) : 1.0f;
}

int player_core_get_state(PlayerCore *pc) {
    if (!pc) return 0;
    return pc->state;
}

uint8_t* player_core_get_video_frame(PlayerCore *pc) {
    if (!pc) return NULL;
    if (!pc->vdb) return NULL;
    return vdb_get_ready_buf(pc->vdb);
}

uint8_t* player_core_get_video_frame_at_time(PlayerCore *pc, double audio_time) {
    if (!pc) return NULL;
    if (!pc->vdb) return NULL;
    return vdb_get_ready_buf_at_time(pc->vdb, audio_time);
}

void player_core_release_video_frame(PlayerCore *pc) {
    if (!pc) return;
    if (!pc->vdb) return;
    vdb_release(pc->vdb);
}

// ========== 播放列表管理实现 ==========


void player_core_set_audio_playlist(PlayerCore *pc, Playlist *playlist) {
    if (!pc) return;
    pc->audio_playlist = playlist;
}

void player_core_set_video_playlist(PlayerCore *pc, Playlist *playlist) {
    if (!pc) return;
    pc->video_playlist = playlist;
}

Playlist* player_core_get_audio_playlist(PlayerCore *pc) {
    return pc ? pc->audio_playlist : NULL;
}

Playlist* player_core_get_video_playlist(PlayerCore *pc) {
    return pc ? pc->video_playlist : NULL;
}

int player_core_play_audio_by_index(PlayerCore *pc, int idx) {
    if (!pc || !pc->audio_playlist) return -1;
    
    PlaylistItem *item = playlist_get_item(pc->audio_playlist, idx);
    if (!item) return -1;
    
    playlist_set_current_idx(pc->audio_playlist, idx);
    return player_core_play_audio(pc, item->file_path);
}

int player_core_play_video_by_index(PlayerCore *pc, int idx, int width, int height) {
    if (!pc || !pc->video_playlist) return -1;
    
    PlaylistItem *item = playlist_get_item(pc->video_playlist, idx);
    if (!item) return -1;
    
    playlist_set_current_idx(pc->video_playlist, idx);
    return player_core_play_video(pc, item->file_path, width, height);
}

int player_core_play_next(PlayerCore *pc) {
    if (!pc) return -1;
    
    if (pc->video_mode && pc->video_playlist) {
        int next_idx = playlist_next(pc->video_playlist);
        if (next_idx >= 0) {
            PlaylistItem *item = playlist_get_current_item(pc->video_playlist);
            if (item) {
                // 使用固定分辨率（避免分辨率变化导致问题）
                player_core_play_video(pc, item->file_path, 800, 450);
            }
        }
        return next_idx;
    } else if (pc->audio_playlist) {
        int next_idx = playlist_next(pc->audio_playlist);
        if (next_idx >= 0) {
            PlaylistItem *item = playlist_get_current_item(pc->audio_playlist);
            if (item) {
                player_core_play_audio(pc, item->file_path);
            }
        }
        return next_idx;
    }
    
    return -1;
}

int player_core_play_prev(PlayerCore *pc) {
    if (!pc) return -1;
    
    if (pc->video_mode && pc->video_playlist) {
        int prev_idx = playlist_prev(pc->video_playlist);
        if (prev_idx >= 0) {
            PlaylistItem *item = playlist_get_current_item(pc->video_playlist);
            if (item) {
                player_core_play_video(pc, item->file_path, 800, 450);
            }
        }
        return prev_idx;
    } else if (pc->audio_playlist) {
        int prev_idx = playlist_prev(pc->audio_playlist);
        if (prev_idx >= 0) {
            PlaylistItem *item = playlist_get_current_item(pc->audio_playlist);
            if (item) {
                player_core_play_audio(pc, item->file_path);
            }
        }
        return prev_idx;
    }
    
    return -1;
}