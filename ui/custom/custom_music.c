#include <string.h>
#include <stdlib.h>
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

static void custom_media_update_song_info(void);

int custom_media_scan_music_dir(const char *dir_path) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_scan_music_dir: dir_path=%s\n", dir_path);
    if (!dir_path || !ctx->audio_playlist) return -1;

    playlist_clear(ctx->audio_playlist);
    int count = playlist_scan_dir(ctx->audio_playlist, dir_path);
    LOGD("custom_media: 从 %s 扫描到 %d 首歌曲\n", dir_path, count);
    return count;
}

int custom_media_parse_lyrics(const char *file_path) {
    MediaContext *ctx = custom_media_get_context();

    for (int i = 0; i < ctx->lyrics_count; i++) {
        free(ctx->lyrics[i]);
        ctx->lyrics[i] = NULL;
    }
    ctx->lyrics_count = 0;
    ctx->current_lyric_idx = -1;

    char lrc_path[512];
    const char *ext = strrchr(file_path, '.');
    if (ext) {
        int basename_len = ext - file_path;
        snprintf(lrc_path, sizeof(lrc_path), "%.*s.lrc", basename_len, file_path);
    } else {
        snprintf(lrc_path, sizeof(lrc_path), "%s.lrc", file_path);
    }

    FILE *fp = fopen(lrc_path, "r");
    if (!fp) return -1;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (line[0] == '[') {
            char *end_bracket = strchr(line, ']');
            if (end_bracket) {
                *end_bracket = '\0';
                char *content = end_bracket + 1;
                if (strlen(content) > 0) {
                    int min = 0, sec = 0;
                    float msec = 0;
                    if (sscanf(line + 1, "%d:%d.%f", &min, &sec, &msec) == 3) {
                        ctx->lyrics_time[ctx->lyrics_count] = min * 60 + sec + msec / 100;
                        ctx->lyrics[ctx->lyrics_count++] = strdup(content);
                    }
                }
            }
        } else {
            ctx->lyrics_time[ctx->lyrics_count] = 0;
            ctx->lyrics[ctx->lyrics_count++] = strdup(line);
        }
        if (ctx->lyrics_count >= MAX_LYRICS_LINES) break;
    }
    fclose(fp);
    LOGD("custom_media: loaded %d lyrics lines from %s\n", ctx->lyrics_count, lrc_path);
    for (int i = 0; i < ctx->lyrics_count && i < 5; i++) {
        LOGD("custom_media: lyric[%d] time=%.2fs text=%s\n", i, ctx->lyrics_time[i], ctx->lyrics[i]);
    }
    if (ctx->lyrics_count > 5) {
        LOGD("custom_media: ... and %d more lyrics\n", ctx->lyrics_count - 5);
    }
    return ctx->lyrics_count;
}

static char s_lyrics_display[2048] = {0};

const char* custom_media_get_current_lyric(void) {
    MediaContext *ctx = custom_media_get_context();
    if (ctx->lyrics_count == 0) return "暂无歌词";

    memset(s_lyrics_display, 0, sizeof(s_lyrics_display));

    int half_lines = LYRICS_DISPLAY_LINES / 2;
    int start_idx = ctx->current_lyric_idx - half_lines;
    if (start_idx < 0) start_idx = 0;

    int end_idx = start_idx + LYRICS_DISPLAY_LINES;
    if (end_idx > ctx->lyrics_count) end_idx = ctx->lyrics_count;

    int pos = 0;
    int remain = (int)sizeof(s_lyrics_display);
    for (int i = start_idx; i < end_idx; i++) {
        const char *color = (i == ctx->current_lyric_idx) ? "#FFFFFF " : "#666666 ";
        int written = snprintf(s_lyrics_display + pos, remain, "%s%s#\n", color, ctx->lyrics[i]);
        if (written < 0 || written >= remain) break;
        pos += written;
        remain -= written;
    }
    return s_lyrics_display;
}

void custom_media_update_lyric_index(double position) {
    MediaContext *ctx = custom_media_get_context();
    if (ctx->lyrics_count == 0) return;

    int new_idx = -1;
    for (int i = 0; i < ctx->lyrics_count; i++) {
        if (ctx->lyrics_time[i] > position) {
            if (i > 0) new_idx = i - 1;
            break;
        }
        new_idx = i;
    }

    if (new_idx >= 0 && new_idx < ctx->lyrics_count && new_idx != ctx->current_lyric_idx) {
        ctx->current_lyric_idx = new_idx;
        if (ctx->ui && ctx->ui->screen_music_label_lyric) {
            lv_obj_set_style_text_font(ctx->ui->screen_music_label_lyric,
                                       custom_font_get_lyric(), LV_PART_MAIN);
            lv_label_set_text(ctx->ui->screen_music_label_lyric, custom_media_get_current_lyric());
        }
    }
}

void custom_media_init_music(void) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_init_music: 初始化音乐功能\n");
    ctx->play_mode = 0;

    /* 重置封面图为默认，避免 playlist_clear 后野指针导致乱码 */
    if (ctx->ui && ctx->ui->screen_music_img_music) {
        lv_img_set_src(ctx->ui->screen_music_img_music, &_1725420839434_alpha_250x250);
    }

    if (!ctx->audio_playlist) {
        ctx->audio_playlist = playlist_create(PLAYLIST_TYPE_AUDIO, MAX_SONGS);
        LOGD("custom_media_init_music: 重新创建音频播放列表\n");
    }

    custom_media_scan_music_dir("/home/debian/music");
}

static void custom_media_update_song_info(void) {
    MediaContext *ctx = custom_media_get_context();

    if (!ctx->ui) return;

    int idx = custom_media_get_current_song_idx();
    if (idx < 0) {
        LOGD("custom_media_update_song_info: 无效索引\n");
        return;
    }

    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item) return;

    if (ctx->ui->screen_music_label_song) {
        /* 使用 FreeType 字体，避免预生成字库缺字导致乱码 */
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_song,
                                   custom_font_get(), LV_PART_MAIN);
        const char *title = item->title && item->title[0] != '\0' ? item->title : NULL;
        lv_label_set_text(ctx->ui->screen_music_label_song,
                          title ? title : "未知歌曲");
    }

    if (ctx->ui->screen_music_label_singer) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_singer,
                                   custom_font_get(), LV_PART_MAIN);
        const char *artist = item->artist && item->artist[0] != '\0' ? item->artist : "未知歌手";
        lv_label_set_text(ctx->ui->screen_music_label_singer, artist);
    }

    if (ctx->ui->screen_music_label_fileName) {
        /* 封面下方显示文件名（不含扩展名） */
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_fileName,
                                   custom_font_get(), LV_PART_MAIN);
        const char *path = item->file_path;
        if (path && path[0]) {
            const char *slash = strrchr(path, '/');
            const char *name = slash ? slash + 1 : path;
            const char *dot = strrchr(name, '.');
            int len = dot ? (int)(dot - name) : (int)strlen(name);
            char buf[256];
            if (len > (int)sizeof(buf) - 1) len = (int)sizeof(buf) - 1;
            memcpy(buf, name, len);
            buf[len] = '\0';
            lv_label_set_text(ctx->ui->screen_music_label_fileName, buf);
        } else {
            lv_label_set_text(ctx->ui->screen_music_label_fileName, "");
        }
    }

    if (ctx->ui->screen_music_label_endTime) {
        char dur_str[16];
        custom_media_format_duration(item->duration_ms, dur_str, sizeof(dur_str));
        lv_label_set_text(ctx->ui->screen_music_label_endTime, dur_str);
    }

    if (ctx->ui->screen_music_label_progressTime) {
        lv_label_set_text(ctx->ui->screen_music_label_progressTime, "00:00");
    }

    if (ctx->ui->screen_music_slider_progressTime) {
        lv_slider_set_value(ctx->ui->screen_music_slider_progressTime, 0, LV_ANIM_OFF);
    }

    if (ctx->ui->screen_music_img_music) {
        if (item->cover_data && item->cover_size > 0 && item->cover_w > 0 && item->cover_h > 0) {
            static lv_img_dsc_t cover_dsc;
            cover_dsc.header.always_zero = 0;
            cover_dsc.header.w = item->cover_w;
            cover_dsc.header.h = item->cover_h;
            cover_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
            cover_dsc.data_size = item->cover_size;
            cover_dsc.data = item->cover_data;
            lv_img_set_src(ctx->ui->screen_music_img_music, &cover_dsc);
        } else {
            lv_img_set_src(ctx->ui->screen_music_img_music, &_1725420839434_alpha_250x250);
        }
        lv_obj_clear_flag(ctx->ui->screen_music_img_music, LV_OBJ_FLAG_HIDDEN);
    }

    /* 更新歌词区域：有歌词显示歌词，无歌词显示"暂无歌词" */
    if (ctx->ui->screen_music_label_lyric) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_lyric,
                                   custom_font_get_lyric(), LV_PART_MAIN);
        lv_label_set_recolor(ctx->ui->screen_music_label_lyric, true);
        lv_label_set_text(ctx->ui->screen_music_label_lyric, custom_media_get_current_lyric());
    }

    LOGD("custom_media_update_song_info: 完成\n");
}

void custom_media_update_ui_display(void) {
    custom_media_update_song_info();
}

int custom_media_play_audio(const char *file) {
    return ipc_media_play_audio(file);
}

int custom_media_play_song_by_index(int idx) {
    MediaContext *ctx = custom_media_get_context();

    LOGD("custom_media_play_song_by_index: idx=%d\n", idx);
    if (!ctx->audio_playlist) return -1;

    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item || !item->file_path) return -1;

    playlist_set_current_idx(ctx->audio_playlist, idx);

    int result = ipc_media_play_audio(item->file_path);

    if (ctx->ui) {
        PlaylistItem *cur = playlist_get_current_item(ctx->audio_playlist);
        if (cur) {
            custom_media_parse_lyrics(cur->file_path);
        }
        custom_media_update_ui_display();
    }

    return result >= 0 ? 0 : -1;
}

int custom_media_play_next(void) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return -1;

    int count = playlist_get_count(ctx->audio_playlist);
    if (count <= 0) return -1;

    int result = playlist_next(ctx->audio_playlist);
    if (result >= 0) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            result = ipc_media_play_audio(item->file_path);
        }
    }

    if (ctx->ui) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            custom_media_parse_lyrics(item->file_path);
        }
        custom_media_update_ui_display();
    }

    return result >= 0 ? 0 : -1;
}

int custom_media_play_prev(void) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return -1;

    int count = playlist_get_count(ctx->audio_playlist);
    if (count <= 0) return -1;

    int result = playlist_prev(ctx->audio_playlist);
    if (result >= 0) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            result = ipc_media_play_audio(item->file_path);
        }
    }

    if (ctx->ui) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            custom_media_parse_lyrics(item->file_path);
        }
        custom_media_update_ui_display();
    }

    return result >= 0 ? 0 : -1;
}

int custom_media_get_current_song_idx(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->audio_playlist ? playlist_get_current_idx(ctx->audio_playlist) : -1;
}

int custom_media_get_song_count(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->audio_playlist ? playlist_get_count(ctx->audio_playlist) : 0;
}

const char* custom_media_get_song_path(int idx) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item ? item->file_path : NULL;
}

char* custom_media_get_song_name(const char *path) {
    if (!path) return NULL;
    const char *slash = strrchr(path, '/');
    const char *name_start = slash ? slash + 1 : path;
    const char *dot = strrchr(name_start, '.');
    int len = dot ? (dot - name_start) : strlen(name_start);
    char *name = malloc(len + 1);
    if (name) {
        strncpy(name, name_start, len);
        name[len] = '\0';
    }
    return name;
}

const char* custom_media_get_file_name(int idx) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_get_file_name: idx=%d\n", idx);
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item || !item->file_path) return "";
    const char *slash = strrchr(item->file_path, '/');
    return slash ? slash + 1 : item->file_path;
}

const char* custom_media_get_song_title(int idx) {
    MediaContext *ctx = custom_media_get_context();
    LOGD("custom_media_get_song_title: idx=%d\n", idx);
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->title ? item->title : "";
}

const char* custom_media_get_song_artist(int idx) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->artist ? item->artist : "";
}

uint8_t* custom_media_get_song_cover(int idx, size_t *size) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item || !item->cover_data || item->cover_size == 0) return NULL;
    if (size) *size = item->cover_size;
    return item->cover_data;
}

const char* custom_media_get_song_album(int idx) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->album ? item->album : "";
}

int64_t custom_media_get_song_duration_ms(int idx) {
    MediaContext *ctx = custom_media_get_context();
    if (!ctx->audio_playlist) return 0;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item ? item->duration_ms : 0;
}

Playlist* custom_media_get_audio_playlist(void) {
    MediaContext *ctx = custom_media_get_context();
    return ctx->audio_playlist;
}

static void music_list_item_click_event(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    LOGD("music_list_item_click_event: clicked item %d\n", idx);

    custom_media_play_song_by_index(idx);
    custom_media_start_progress_update();
    custom_media_update_ui_display();
}

void custom_media_init_music_list(lv_ui *ui) {
    MediaContext *ctx = custom_media_get_context();

    if (!ui) {
        LOGD("custom_media_init_music_list: ui 为空\n");
        return;
    }
    if (!ui->screen_music_list_music) {
        LOGD("custom_media_init_music_list: screen_music_list_music 为空\n");
        return;
    }
    if (!ctx->audio_playlist) {
        LOGD("custom_media_init_music_list: audio_playlist 为空\n");
        return;
    }

    LOGD("custom_media_init_music_list: 初始化音乐列表\n");

    lv_obj_clean(ui->screen_music_list_music);

    int count = playlist_get_count(ctx->audio_playlist);
    LOGD("custom_media_init_music_list: 找到 %d 首歌曲\n", count);

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
        PlaylistItem *item = playlist_get_item(ctx->audio_playlist, i);
        if (!item || !item->file_path) {
            LOGD("custom_media_init_music_list: item %d is NULL or has no file_path\n", i);
            continue;
        }

        const char *name = item->title && item->title[0] != '\0' ? item->title : item->file_path;
        const char *display_name = strrchr(name, '/');
        if (display_name) display_name++;
        else display_name = name;

        LOGD("custom_media_init_music_list: 添加歌曲 %d: %s\n", i, display_name);

        lv_obj_t *btn = lv_list_add_btn(ui->screen_music_list_music, LV_SYMBOL_AUDIO, display_name);
        lv_obj_t* img = lv_obj_get_child(btn, 0);
        lv_obj_set_style_text_font(img, &lv_font_montserratMedium_16, LV_PART_MAIN);
        lv_img_set_src(img, LV_SYMBOL_AUDIO);

        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        lv_obj_add_style(btn, &style_list_btn, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn, music_list_item_click_event, LV_EVENT_CLICKED, NULL);
    }

    LOGD("custom_media_init_music_list: 完成, 状态=%d, 播放模式=%d\n",
         custom_media_get_state(), custom_media_get_play_mode());

    if (custom_media_get_state() == 1) {
        LOGD("custom_media_init_music_list: 警告！音乐正在播放，停止中...\n");
        custom_media_stop();
        LOGD("custom_media_init_music_list: 停止后, 状态=%d\n", custom_media_get_state());
    }
}