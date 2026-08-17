#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "custom_media.h"
#include "ipc_media.h"
#include "playlist.h"
#include "custom_font.h"
#include "lvgl.h"
#include "gui_guider.h"

int custom_media_scan_video_dir(const char *dir_path) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_scan_video_dir: dir_path=%s\n", dir_path);
    if (!dir_path || !ctx->video_playlist) return -1;

    playlist_clear(ctx->video_playlist);
    int count = playlist_scan_dir(ctx->video_playlist, dir_path);
    LOGD("custom_media: 从 %s 扫描到 %d 个视频\n", dir_path, count);
    return count;
}

void custom_media_init_video(void) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_init_video: 初始化视频功能\n");
    ctx->play_mode = 1;

    if (!ctx->video_playlist) {
        ctx->video_playlist = playlist_create(PLAYLIST_TYPE_VIDEO, MAX_VIDEOS);
        LOGD("custom_media_init_video: 重新创建视频播放列表\n");
    }

    custom_media_scan_video_dir("/home/debian/video");
}

uint8_t* custom_media_get_video_frame(void) {
    static uint8_t frame_buffer[VIDEO_WIDTH * VIDEO_HEIGHT * 2];
    if (ipc_media_get_video_frame(frame_buffer, sizeof(frame_buffer)) == 0) {
        return frame_buffer;
    }
    return NULL;
}

void custom_media_release_video_frame(void) {
}

void custom_media_update_video_stats(void);

void custom_media_update_video_frame(lv_timer_t *timer) {
    MediaContext *ctx = custom_media_get_context();
    static int frame_count = 0;

    if (!ctx) return;
    if (!ctx->ui || !ctx->ui->screen_video_img_video) return;

    frame_count++;

    if (frame_count % 12 == 0) {
        if (custom_media_get_state() != 1) {
            frame_count = 0;
            return;
        }
    }

    uint8_t *frame = custom_media_get_video_frame();
    if (frame) {
        static lv_img_dsc_t video_img_dsc = {
            .header.always_zero = 0,
            .header.w = VIDEO_WIDTH,
            .header.h = VIDEO_HEIGHT,
            .header.cf = LV_IMG_CF_TRUE_COLOR,
            .data_size = VIDEO_WIDTH * VIDEO_HEIGHT * 2,
            .data = NULL
        };
        video_img_dsc.data = frame;

        lv_img_set_src(ctx->ui->screen_video_img_video, &video_img_dsc);
        lv_obj_invalidate(ctx->ui->screen_video_img_video);
    }

    if (frame_count % 12 == 0) {
        custom_media_update_video_stats();
    }
}

void custom_media_update_video_stats(void) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->ui) return;

    int state = 0;
    double position = 0.0, duration = 0.0;
    ipc_media_get_playback_info(&state, &position, &duration);

    if (ctx->ui->screen_video_bar_time && duration > 0) {
        int progress = (int)(position / duration * 100);
        lv_bar_set_value(ctx->ui->screen_video_bar_time, progress, LV_ANIM_OFF);
    }

    if (ctx->ui->screen_video_label_progressTime) {
        char time_str[32];
        int pos_min = (int)(position / 60);
        int pos_sec = (int)(position - pos_min * 60);
        int dur_min = (int)(duration / 60);
        int dur_sec = (int)(duration - dur_min * 60);
        snprintf(time_str, sizeof(time_str), "%02d:%02d/%02d:%02d",
                 pos_min, pos_sec, dur_min, dur_sec);
        lv_label_set_text(ctx->ui->screen_video_label_progressTime, time_str);
    }
}

void custom_media_update_video_progress(void) {
    custom_media_update_video_stats();
}

void custom_media_update_video_time_display(void) {
    // 已合并到 custom_media_update_video_stats
}

void custom_media_update_video_filename(const char *file_name) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->ui || !ctx->ui->screen_video_label_fileName || !file_name) return;

    const char *name = strrchr(file_name, '/');
    if (name) name++;
    else name = file_name;

    lv_obj_set_style_text_font(ctx->ui->screen_video_label_fileName, custom_font_get(), LV_PART_MAIN);
    lv_label_set_text(ctx->ui->screen_video_label_fileName, name);
}

int custom_media_play_video_by_index(int idx) {
    MediaContext *ctx = custom_media_get_context();

    LOGD("custom_media_play_video_by_index: idx=%d\n", idx);

    if (!ctx->video_playlist) {
        LOGD("custom_media_play_video_by_index: video_playlist 为空\n");
        return -1;
    }

    PlaylistItem *item = playlist_get_item(ctx->video_playlist, idx);
    if (!item) {
        LOGD("custom_media_play_video_by_index: item 为空\n");
        return -1;
    }

    LOGD("custom_media_play_video_by_index: 播放视频 %s\n", item->file_path);

    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
    }

    usleep(50000);

    playlist_set_current_idx(ctx->video_playlist, idx);

    custom_media_update_video_filename(item->file_path);

    if (ctx->ui && ctx->ui->screen_video_img_video) {
        lv_obj_set_size(ctx->ui->screen_video_img_video, VIDEO_WIDTH, VIDEO_HEIGHT);
        lv_obj_center(ctx->ui->screen_video_img_video);
        LOGD("custom_media_play_video_by_index: 图像控件大小设置为 %dx%d\n", VIDEO_WIDTH, VIDEO_HEIGHT);
    }

    int result = ipc_media_play_video(item->file_path, VIDEO_WIDTH, VIDEO_HEIGHT);
    LOGD("custom_media_play_video_by_index: 结果=%d\n", result);

    if (result != 0) {
        LOGD("custom_media_play_video_by_index: 视频播放失败，可能是格式不支持\n");
        if (ctx->ui && ctx->ui->screen_video_label_fileName) {
            lv_label_set_text(ctx->ui->screen_video_label_fileName, "视频格式不支持");
        }
        if (ctx->video_frame_timer) {
            lv_timer_del(ctx->video_frame_timer);
            ctx->video_frame_timer = NULL;
        }
        return -1;
    }

    int frame_interval_ms = 30;
    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, frame_interval_ms, NULL);

    if (ctx->ui && ctx->ui->screen_video_img_video) {
        usleep(100000);
        lv_obj_invalidate(ctx->ui->screen_video_img_video);
    }

    return result;
}

int custom_media_play_next_video(void) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->video_playlist) return -1;

    int count = playlist_get_count(ctx->video_playlist);
    if (count <= 0) return -1;

    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        usleep(50000);
    }

    int result = playlist_next(ctx->video_playlist);
    if (result >= 0) {
        PlaylistItem *item = playlist_get_current_item(ctx->video_playlist);
        if (item) {
            result = ipc_media_play_video(item->file_path, VIDEO_WIDTH, VIDEO_HEIGHT);
        }
    }

    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, 30, NULL);

    return result >= 0 ? 0 : -1;
}

int custom_media_play_prev_video(void) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->video_playlist) return -1;

    int count = playlist_get_count(ctx->video_playlist);
    if (count <= 0) return -1;

    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        usleep(50000);
    }

    int result = playlist_prev(ctx->video_playlist);
    if (result >= 0) {
        PlaylistItem *item = playlist_get_current_item(ctx->video_playlist);
        if (item) {
            result = ipc_media_play_video(item->file_path, VIDEO_WIDTH, VIDEO_HEIGHT);
        }
    }

    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, 30, NULL);

    return result >= 0 ? 0 : -1;
}

int custom_media_get_current_video_idx(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->video_playlist ? playlist_get_current_idx(ctx->video_playlist) : -1;
}

int custom_media_get_video_count(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->video_playlist ? playlist_get_count(ctx->video_playlist) : 0;
}

const char* custom_media_get_video_path(int idx) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->video_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->video_playlist, idx);
    return item ? item->file_path : NULL;
}

Playlist* custom_media_get_video_playlist(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->video_playlist;
}

static void video_list_item_click_event(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    LOGD("video_list_item_click_event: clicked item %d\n", idx);

    int result = custom_media_play_video_by_index(idx);

    MediaContext *ctx = custom_media_get_context();
    if (result == 0 && ctx->ui && ctx->ui->screen_video_btn_playing_label) {
        lv_label_set_text(ctx->ui->screen_video_btn_playing_label, LV_SYMBOL_PAUSE);
        LOGD("video_list_item_click_event: 更新播放按钮为暂停图标\n");
    }
}

void custom_media_init_video_list(lv_ui *ui) {
    MediaContext *ctx = custom_media_get_context();

    LOGD("custom_media_init_video_list: 调用, 状态=%d, 播放模式=%d\n",
         custom_media_get_state(), custom_media_get_play_mode());

    if (!ui || !ui->screen_video_list_video) {
        LOGD("custom_media_init_video_list: ui 或 screen_video_list_video 为空\n");
        return;
    }

    if (!ctx->video_playlist) {
        LOGD("custom_media_init_video_list: video_playlist 为空\n");
        return;
    }

    if (custom_media_get_state() == 1) {
        LOGD("custom_media_init_video_list: stopping audio before init\n");
        custom_media_stop();
    }

    LOGD("custom_media_init_video_list: 初始化视频列表\n");
    lv_obj_clean(ui->screen_video_list_video);

    int count = playlist_get_count(ctx->video_playlist);
    LOGD("custom_media_init_video_list: 找到 %d 个视频\n", count);

    static lv_style_t style_list_btn;
    ui_init_style(&style_list_btn);
    lv_style_set_pad_top(&style_list_btn, 5);
    lv_style_set_pad_left(&style_list_btn, 10);
    lv_style_set_pad_right(&style_list_btn, 0);
    lv_style_set_pad_bottom(&style_list_btn, 10);
    lv_style_set_border_width(&style_list_btn, 1);
    lv_style_set_border_opa(&style_list_btn, 255);
    lv_style_set_border_color(&style_list_btn, lv_color_hex(0x6f6969));
    lv_style_set_border_side(&style_list_btn, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_list_btn, lv_color_hex(0xfef6ea));
    lv_style_set_text_font(&style_list_btn, custom_font_get());
    lv_style_set_text_opa(&style_list_btn, 255);
    lv_style_set_radius(&style_list_btn, 0);
    lv_style_set_bg_opa(&style_list_btn, 0);

    for (int i = 0; i < count; i++) {
        PlaylistItem *item = playlist_get_item(ctx->video_playlist, i);
        if (!item || !item->file_path) continue;

        const char *name = item->title && item->title[0] != '\0' ? item->title : item->file_path;
        const char *display_name = strrchr(name, '/');
        if (display_name) display_name++;
        else display_name = name;

        LOGD("custom_media_init_video_list: 添加视频 %d: %s\n", i, display_name);

        lv_obj_t *btn = lv_list_add_btn(ui->screen_video_list_video, LV_SYMBOL_VIDEO, display_name);
        lv_obj_t* img = lv_obj_get_child(btn, 0);
        lv_obj_set_style_text_font(img, &lv_font_montserratMedium_16, LV_PART_MAIN);
        lv_img_set_src(img, LV_SYMBOL_VIDEO);

        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        lv_obj_add_style(btn, &style_list_btn, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn, video_list_item_click_event, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_invalidate(ui->screen_video_list_video);

    LOGD("custom_media_init_video_list: 完成\n");
}