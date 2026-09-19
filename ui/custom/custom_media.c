#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include "custom_media.h"
#include "ipc_media.h"
#include "playlist.h"
#include "media_metadata.h"
#include "custom_font.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "ipc_audio.h"

static MediaContext g_media_ctx = {0};

// 音乐服务进程管理
static pid_t g_media_svc_pid = 0;

// 前向声明
static void progress_update_callback(lv_timer_t *timer);

/**
 * @brief 获取全局媒体上下文
 */
MediaContext* custom_media_get_context(void) {
    return &g_media_ctx;
}

static void on_playback_complete(void *user_data)
{
    MediaContext *ctx = (MediaContext *)user_data;
    if (ctx && !ctx->is_deinitializing) {
        ctx->playback_just_finished = true;
        LOGD("on_playback_complete: 收到播放完成信号\n");
    }
}

/**
 * @brief 初始化媒体播放核心模块
 */
void custom_media_init(lv_ui *ui) {
    MediaContext *ctx = custom_media_get_context();
    if (!ui) return;
    ctx->ui = ui;
    LOGD("custom_media_init: ui=%p\n", ctx->ui);

    if (g_media_svc_pid <= 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./media_service", "media_service", NULL);
            perror("execl media_service");
            _exit(1);
        } else if (pid > 0) {
            g_media_svc_pid = pid;
            printf("[custom_media] media_service started pid=%d\n", pid);
        }

        if (ipc_media_init(BUS_ADDRESS) == 0) {
            ipc_media_set_complete_callback(on_playback_complete, ctx);
        } else {
            fprintf(stderr, "[custom_media] IPC Media init failed\n");
        }
    }

    if (!ctx->audio_playlist) {
        ctx->audio_playlist = playlist_create(PLAYLIST_TYPE_AUDIO, MAX_SONGS);
        LOGD("custom_media_init: 创建音频播放列表\n");
    }

    if (!ctx->video_playlist) {
        ctx->video_playlist = playlist_create(PLAYLIST_TYPE_VIDEO, MAX_VIDEOS);
        LOGD("custom_media_init: 创建视频播放列表\n");
    }

    if (!ctx->progress_timer) {
        ctx->progress_timer = lv_timer_create(progress_update_callback, 500, NULL);
        lv_timer_pause(ctx->progress_timer);
        LOGD("custom_media_init: 创建进度更新定时器\n");
    }

    ctx->playback_just_finished = false;
    ctx->last_pos = 0.0;
    ctx->is_deinitializing = false;

    LOGD("custom_media_init: 初始化完成\n");
}

/**
 * @brief 进度更新定时器回调
 */
static void progress_update_callback(lv_timer_t *timer) {
    MediaContext *ctx = custom_media_get_context();

    if (!ctx->ui) return;
    if (ctx->is_deinitializing) return;

    int play_mode = custom_media_get_play_mode();
    if (play_mode == 1) return;

    int state = 0;
    double pos = 0.0, duration = 0.0;
    ipc_media_get_playback_info(&state, &pos, &duration);

    if (state != 1) return;

    char pos_str[16];
    custom_media_format_duration((int64_t)(pos * 1000), pos_str, sizeof(pos_str));
    if (ctx->ui->screen_music_label_progressTime) {
        lv_label_set_text(ctx->ui->screen_music_label_progressTime, pos_str);
    }

    char dur_str[16];
    custom_media_format_duration((int64_t)(duration * 1000), dur_str, sizeof(dur_str));
    if (ctx->ui->screen_music_label_endTime) {
        lv_label_set_text(ctx->ui->screen_music_label_endTime, dur_str);
    }

    if (duration > 0) {
        int progress = (int)((pos / duration) * 100);
        if (ctx->ui->screen_music_slider_progressTime) {
            lv_slider_set_value(ctx->ui->screen_music_slider_progressTime, progress, LV_ANIM_OFF);
        }
    }

    custom_media_update_lyric_index(pos);

    if (ctx->playback_just_finished && state == 0) {
        ctx->playback_just_finished = false;
        LOGD("progress_update_callback: 播放完成，播放下一首\n");
        custom_media_play_next();
        custom_media_update_ui_display();
    }

    ctx->last_pos = pos;
}

void custom_media_start_progress_update(void) {
    MediaContext *ctx = custom_media_get_context();
    if (ctx->progress_timer) {
        lv_timer_resume(ctx->progress_timer);
    }
}

void custom_media_stop_progress_update(void) {
    MediaContext *ctx = custom_media_get_context();
    if (ctx->progress_timer) {
        lv_timer_pause(ctx->progress_timer);
    }
}

/**
 * @brief 屏幕切换时的轻量清理：停止播放和定时器，但保留基础设施（IPC、playlist、media_service）
 */
void custom_media_cleanup_screen(void) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_cleanup_screen: 开始轻量清理\n");

    ctx->is_deinitializing = true;
    usleep(20000);

    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        LOGD("custom_media_cleanup_screen: 视频帧定时器已停止\n");
    }

    if (ctx->progress_timer) {
        lv_timer_pause(ctx->progress_timer);
        LOGD("custom_media_cleanup_screen: 进度定时器已暂停\n");
    }

    if (ctx->play_mode == 1 && ctx->ui && ctx->ui->screen_video_img_video) {
        lv_img_set_src(ctx->ui->screen_video_img_video, &_noVideo_alpha_800x450);
    }

    /* 退出时重置音乐封面为默认，避免 cover_dsc 野指针导致乱码 */
    if (ctx->ui && ctx->ui->screen_music_img_music) {
        lv_img_set_src(ctx->ui->screen_music_img_music, &_1725420839434_alpha_250x250);
    }

    ipc_media_stop();

    ctx->playback_just_finished = false;
    ctx->last_pos = 0.0;
    ctx->is_deinitializing = false;

    /* 关闭多媒体界面时停止 media_service 进程 */
    if (g_media_svc_pid > 0) {
        kill(g_media_svc_pid, SIGTERM);
        int retries = 0;
        while (waitpid(g_media_svc_pid, NULL, WNOHANG) == 0 && retries < 10) {
            usleep(100000);
            retries++;
        }
        if (retries >= 10) {
            kill(g_media_svc_pid, SIGKILL);
            waitpid(g_media_svc_pid, NULL, 0);
        }
        printf("[custom_media] media_service stopped pid=%d\n", g_media_svc_pid);
        g_media_svc_pid = 0;
        ipc_media_deinit();
    }

    LOGD("custom_media_cleanup_screen: 完成\n");
}

/**
 * @brief 反初始化媒体播放核心模块（仅 app 退出时调用）
 */
void custom_media_deinit(void) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_deinit: 开始反初始化\n");

    ctx->is_deinitializing = true;
    usleep(20000);

    LOGD("custom_media_deinit: 停止进度定时器\n");
    if (ctx->progress_timer) {
        lv_timer_del(ctx->progress_timer);
        ctx->progress_timer = NULL;
        LOGD("custom_media_deinit: 进度定时器已停止\n");
    }

    LOGD("custom_media_deinit: 停止视频帧定时器 timer=%p\n", ctx->video_frame_timer);
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        LOGD("custom_media_deinit: 视频帧定时器已停止\n");
    } else {
        LOGD("custom_media_deinit: 视频帧定时器为NULL\n");
    }

    if (ctx->play_mode == 1 && ctx->ui && ctx->ui->screen_video_img_video) {
        LOGD("custom_media_deinit: 清空视频图像控件\n");
        lv_img_set_src(ctx->ui->screen_video_img_video, NULL);
    }

    ctx->playback_just_finished = false;
    ctx->last_pos = 0.0;
    ctx->ui = NULL;
    usleep(20000);

    ipc_media_stop();

    for (int i = 0; i < ctx->lyrics_count; i++) {
        free(ctx->lyrics[i]);
        ctx->lyrics[i] = NULL;
    }
    ctx->lyrics_count = 0;
    ctx->current_lyric_idx = -1;

    if (ctx->audio_playlist) {
        playlist_destroy(ctx->audio_playlist);
        ctx->audio_playlist = NULL;
    }
    if (ctx->video_playlist) {
        playlist_destroy(ctx->video_playlist);
        ctx->video_playlist = NULL;
    }

    LOGD("custom_media_deinit: done\n");

    ipc_media_deinit();

    if (g_media_svc_pid > 0) {
        kill(g_media_svc_pid, SIGKILL);
        int retries = 0;
        while (waitpid(g_media_svc_pid, NULL, WNOHANG) == 0 && retries < 10) {
            usleep(100000);
            retries++;
        }
        g_media_svc_pid = 0;
    }
}

void custom_media_format_duration(int64_t duration_ms, char *buf, size_t buf_size) {
    media_metadata_format_duration_short(duration_ms, buf, buf_size);
}

int custom_media_play_video(const char *file, int width, int height) {
    return ipc_media_play_video(file, width, height);
}

void custom_media_pause(void) {
    int play_mode = custom_media_get_play_mode();
    LOGD("custom_media_pause: 播放模式=%d\n", play_mode);
    ipc_media_pause();
}

void custom_media_resume(void) {
    int play_mode = custom_media_get_play_mode();
    int state = custom_media_get_state();
    LOGD("custom_media_resume: 状态=%d, 播放模式=%d\n", state, play_mode);

    if (state == 0 && play_mode == 1) {
        LOGD("custom_media_resume: 视频模式下空闲状态，忽略\n");
        return;
    }

    ipc_media_resume();
}

void custom_media_set_play_mode(int mode) {
    MediaContext *ctx = custom_media_get_context();
    ctx->play_mode = mode;
}

int custom_media_get_play_mode(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->play_mode;
}

void custom_media_stop(void) {
    MediaContext *ctx = custom_media_get_context();

    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
    }

    ipc_media_stop();
}

void custom_media_seek(double sec) {
    ipc_media_seek(sec);
}

double custom_media_get_position(void) {
    return ipc_media_get_position();
}

double custom_media_get_duration(void) {
    return ipc_media_get_duration();
}

int custom_media_get_state(void) {
    return ipc_media_get_state();
}

void custom_media_set_volume(float volume) {
    ipc_audio_set_volume(volume);
}

float custom_media_get_volume(void) {
    return ipc_audio_get_volume();
}