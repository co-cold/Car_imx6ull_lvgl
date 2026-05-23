#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "custom_media.h"
#include "player_core.h"
#include "playlist.h"
#include "media_metadata.h"
#include "custom_font.h"
#include "lvgl.h"
#include "gui_guider.h"

// 配置常量
#define MAX_SONGS 50
#define MAX_VIDEOS 20
#define MAX_LYRICS_LINES 200
#define LYRICS_DISPLAY_LINES 5
#define LYRICS_FONT_SIZE_NORMAL 16
#define LYRICS_FONT_SIZE_HIGHLI 24
#define VIDEO_WIDTH 800
#define VIDEO_HEIGHT 450

// 媒体播放上下文结构体
typedef struct {
    PlayerCore *pc;                     // 播放器核心
    lv_ui *ui;                          // LVGL UI 指针
    lv_timer_t *progress_timer;         // 进度更新定时器
    lv_timer_t *video_frame_timer;      // 视频帧更新定时器
    
    Playlist *audio_playlist;           // 音频播放列表
    Playlist *video_playlist;           // 视频播放列表
    
    int play_mode;                      // 当前播放模式：0-音乐模式，1-视频模式
    
    // 歌词数据
    char *lyrics[MAX_LYRICS_LINES];
    double lyrics_time[MAX_LYRICS_LINES];
    int lyrics_count;
    int current_lyric_idx;
} MediaContext;

// 全局媒体上下文
static MediaContext g_media_ctx = {0};

// 前向声明
static void progress_update_callback(lv_timer_t *timer);
static uint8_t* get_song_cover(MediaContext *ctx, int idx, size_t *size);
static MediaContext* get_context(void);

// 公共函数前向声明（解决调用顺序问题）
int custom_media_get_play_mode(void);
uint8_t* custom_media_get_song_cover(int idx, size_t *size);

/**
 * @brief 获取媒体上下文
 */
static MediaContext* get_context(void) {
    return &g_media_ctx;
}

/**
 * @brief 扫描音乐目录
 */
int custom_media_scan_music_dir(const char *dir_path) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_scan_music_dir: 开始扫描音乐目录, dir_path=%s\n", dir_path);
    if (!dir_path || !ctx->audio_playlist) return -1;
    
    // 清空现有列表
    playlist_clear(ctx->audio_playlist);
    
    // 使用播放列表模块扫描目录
    int count = playlist_scan_dir(ctx->audio_playlist, dir_path);
    
    LOGD("custom_media: 从 %s 扫描到 %d 首歌曲\n", dir_path, count);
    return count;
}

/**
 * @brief 扫描视频目录
 */
int custom_media_scan_video_dir(const char *dir_path) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_scan_video_dir: 开始扫描视频目录, dir_path=%s\n", dir_path);
    if (!dir_path || !ctx->video_playlist) return -1;
    
    // 清空现有列表
    playlist_clear(ctx->video_playlist);
    
    // 使用播放列表模块扫描目录
    int count = playlist_scan_dir(ctx->video_playlist, dir_path);
    
    LOGD("custom_media: 从 %s 扫描到 %d 个视频\n", dir_path, count);
    return count;
}

/**
 * @brief 解析歌词文件
 */
int custom_media_parse_lyrics(const char *file_path) {
    MediaContext *ctx = get_context();
    
    // 清空现有歌词
    for (int i = 0; i < ctx->lyrics_count; i++) {
        free(ctx->lyrics[i]);
        ctx->lyrics[i] = NULL;
    }
    ctx->lyrics_count = 0;
    ctx->current_lyric_idx = -1;
    
    // 尝试加载同名 .lrc 文件（移除音频扩展名）
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
        // 去除换行符
        line[strcspn(line, "\r\n")] = '\0';
        
        // 跳过空行
        if (strlen(line) == 0) continue;
        
        // 解析标准 .lrc 格式：[mm:ss.xx]歌词内容
        if (line[0] == '[') {
            // 找到时间标签结束的 ]
            char *end_bracket = strchr(line, ']');
            if (end_bracket) {
                *end_bracket = '\0'; // 分隔时间标签和歌词
                char *content = end_bracket + 1;
                if (strlen(content) > 0) {
                    // 提取时间标签（用于后续同步）
                    int min = 0, sec = 0;
                    float msec = 0;
                    if (sscanf(line + 1, "%d:%d.%f", &min, &sec, &msec) == 3) {
                        // 计算时间戳（秒）
                        ctx->lyrics_time[ctx->lyrics_count] = min * 60 + sec + msec / 100;
                        // 存储歌词内容
                        ctx->lyrics[ctx->lyrics_count++] = strdup(content);
                    }
                }
            }
        } else {
            // 普通歌词行（无时间标签）
            ctx->lyrics_time[ctx->lyrics_count] = 0;
            ctx->lyrics[ctx->lyrics_count++] = strdup(line);
        }
        
        if (ctx->lyrics_count >= MAX_LYRICS_LINES) break;
    }
    
    fclose(fp);
    LOGD("custom_media: loaded %d lyrics lines from %s\n", ctx->lyrics_count, lrc_path);
    
    // 打印前5句歌词用于调试
    for (int i = 0; i < ctx->lyrics_count && i < 5; i++) {
        LOGD("custom_media: lyric[%d] time=%.2fs, text=%s\n", i, ctx->lyrics_time[i], ctx->lyrics[i]);
    }
    
    if (ctx->lyrics_count > 5) {
        LOGD("custom_media: ... and %d more lyrics\n", ctx->lyrics_count - 5);
    }
    
    return ctx->lyrics_count;
}

// 歌词显示缓冲区（保持静态，避免频繁分配）
static char s_lyrics_display[2048] = {0};

/**
 * @brief 获取当前歌词（多行显示版本）
 */
const char* custom_media_get_current_lyric(void) {
    MediaContext *ctx = get_context();
    
    if (ctx->lyrics_count == 0) {
        return "暂无歌词";
    }
    
    // 构建多行歌词显示
    memset(s_lyrics_display, 0, sizeof(s_lyrics_display));
    
    // 计算显示范围：当前歌词在中间
    int half_lines = LYRICS_DISPLAY_LINES / 2;
    int start_idx = ctx->current_lyric_idx - half_lines;
    if (start_idx < 0) start_idx = 0;
    
    int end_idx = start_idx + LYRICS_DISPLAY_LINES;
    if (end_idx > ctx->lyrics_count) end_idx = ctx->lyrics_count;
    
    // 生成带样式的多行歌词
    for (int i = start_idx; i < end_idx; i++) {
        if (i == ctx->current_lyric_idx) {
            // 当前播放的歌词：白色高亮（市面上常见配置）
            strcat(s_lyrics_display, "#FFFFFF ");  // 白色高亮
            strcat(s_lyrics_display, ctx->lyrics[i]);
            strcat(s_lyrics_display, "\n");
        } else {
            // 其他歌词：淡灰色显示（市面上常见配置）
            strcat(s_lyrics_display, "#666666 ");  // 淡灰色
            strcat(s_lyrics_display, ctx->lyrics[i]);
            strcat(s_lyrics_display, "\n");
        }
    }
    
    return s_lyrics_display;
}

/**
 * @brief 更新歌词索引（根据播放进度）
 */
void custom_media_update_lyric_index(double position) {
    MediaContext *ctx = get_context();
    
    if (ctx->lyrics_count == 0) return;
    
    // 找到当前播放位置对应的歌词
    // 从当前位置向后查找，找到第一个时间戳大于当前位置的歌词，取前一个
    int new_idx = -1;
    for (int i = 0; i < ctx->lyrics_count; i++) {
        if (ctx->lyrics_time[i] > position) {
            if (i > 0) {
                new_idx = i - 1;
            }
            break;
        }
        new_idx = i;
    }
    
    if (new_idx >= 0 && new_idx < ctx->lyrics_count && new_idx != ctx->current_lyric_idx) {
        ctx->current_lyric_idx = new_idx;
        if (ctx->ui && ctx->ui->screen_music_label_lyric) {
            // 更新多行歌词显示
            lv_label_set_text(ctx->ui->screen_music_label_lyric, custom_media_get_current_lyric());
        }
    }
}



/**
 * @brief 初始化媒体播放核心模块
 * @param ui LVGL UI 指针
 */
void custom_media_init(lv_ui *ui) {
    MediaContext *ctx = get_context();
    
    if (!ui) return;
    
    ctx->ui = ui;
    LOGD("custom_media_init: ui=%p\n", ctx->ui);
    
    // 确保播放器核心存在
    if (!ctx->pc) {
        ctx->pc = player_core_create();
        LOGD("custom_media_init: 创建播放器核心\n");
    }
    
    // 确保音频播放列表存在
    if (!ctx->audio_playlist) {
        ctx->audio_playlist = playlist_create(PLAYLIST_TYPE_AUDIO, MAX_SONGS);
        player_core_set_audio_playlist(ctx->pc, ctx->audio_playlist);
        LOGD("custom_media_init: 创建音频播放列表\n");
    }
    
    // 确保视频播放列表存在
    if (!ctx->video_playlist) {
        ctx->video_playlist = playlist_create(PLAYLIST_TYPE_VIDEO, MAX_VIDEOS);
        player_core_set_video_playlist(ctx->pc, ctx->video_playlist);
        LOGD("custom_media_init: 创建视频播放列表\n");
    }
    
    // 确保进度更新定时器存在
    if (!ctx->progress_timer) {
        ctx->progress_timer = lv_timer_create(progress_update_callback, 100, NULL);
        lv_timer_pause(ctx->progress_timer);
        LOGD("custom_media_init: 创建进度更新定时器\n");
    }
    
    LOGD("custom_media_init: 初始化完成\n");
}

/**
 * @brief 初始化音乐播放功能
 */
void custom_media_init_music(void) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_init_music: 初始化音乐功能\n");
    
    // 设置播放模式为音乐模式
    ctx->play_mode = 0;
    
    // 扫描音乐目录（播放列表已在 custom_media_init 中创建）
    custom_media_scan_music_dir("/home/debian/music");
}

/**
 * @brief 初始化视频播放功能
 */
void custom_media_init_video(void) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_init_video: 初始化视频功能\n");
    
    // 设置播放模式为视频模式
    ctx->play_mode = 1;
    
    // 扫描视频目录（播放列表已在 custom_media_init 中创建）
    custom_media_scan_video_dir("/home/debian/video");
}

/**
 * @brief 更新所有UI显示（歌曲信息、播放状态、歌词等）
 */
void custom_media_update_ui_display(void) {
    MediaContext *ctx = get_context();
    if (!ctx->ui) return;
    
    int idx = custom_media_get_current_song_idx();
    int count = custom_media_get_song_count();
    if (idx < 0 || idx >= count) return;
    
    // 更新播放按钮状态
    if (ctx->ui->screen_music_btn_playing) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_btn_playing, custom_font_get(), LV_PART_MAIN);
        int state = custom_media_get_state();
        lv_label_set_text(ctx->ui->screen_music_btn_playing_label, state == 1 ? "暂停" : "播放");
    }
    
    // 更新文件名显示
    const char *file_name = custom_media_get_file_name(idx);
    if (ctx->ui->screen_music_label_fileName && file_name) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_fileName, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_fileName, file_name);
    }
    
    // 更新歌曲名显示（优先使用标题，若无则使用专辑名，最后使用文件名）
    const char *song_title = custom_media_get_song_title(idx);
    if (!song_title || song_title[0] == '\0') {
        song_title = custom_media_get_song_album(idx);
    }
    if (!song_title || song_title[0] == '\0') {
        song_title = custom_media_get_file_name(idx);
    }
    if (ctx->ui->screen_music_label_song) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_song, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_song, song_title && song_title[0] != '\0' ? song_title : "未知歌曲");
    }
    
    // 更新歌手显示
    const char *song_artist = custom_media_get_song_artist(idx);
    if (ctx->ui->screen_music_label_singer) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_singer, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_singer, song_artist && song_artist[0] != '\0' ? song_artist : "未知歌手");
    }
    
    // 更新歌词显示
    const char *lyric = custom_media_get_current_lyric();
    if (ctx->ui->screen_music_label_lyric) {
        // 设置字体（使用歌词专用字体）
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_lyric, custom_font_get_lyric(), LV_PART_MAIN);
        // 启用彩色文本支持
        lv_label_set_recolor(ctx->ui->screen_music_label_lyric, true);
        // 设置自动换行
        lv_label_set_long_mode(ctx->ui->screen_music_label_lyric, LV_LABEL_LONG_WRAP);
        // 设置文本水平居中
        lv_obj_set_style_text_align(ctx->ui->screen_music_label_lyric, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_lyric, lyric ? lyric : "暂无歌词");
    }
}

/**
 * @brief 更新当前歌曲信息显示
 * @param idx 歌曲索引
 */
static void custom_media_update_song_info(int idx) {
    MediaContext *ctx = get_context();
    int count = custom_media_get_song_count();
    LOGD("custom_media_update_song_info: idx=%d, ctx->ui=%p, count=%d\n", idx, ctx->ui, count);
    if (!ctx->ui || idx < 0 || idx >= count) {
        LOGD("custom_media_update_song_info: invalid parameters, returning\n");
        return;
    }
    
    // 更新文件名显示
    const char *file_name = custom_media_get_file_name(idx);
    if (ctx->ui->screen_music_label_fileName) {
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_fileName, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_fileName, file_name);
    }
    
    // 更新封面显示
    if (ctx->ui->screen_music_img_music) {
        size_t cover_size = 0;
        uint8_t *cover_data = custom_media_get_song_cover(idx, &cover_size);
        LOGD("custom_media_update_song_info: cover_data=%p, cover_size=%zu\n", cover_data, cover_size);
        
        if (cover_data && cover_size > 0) {
                // 检测图片格式
                const char *ext = "png";
                if (cover_size >= 2 && cover_data[0] == 0xFF && cover_data[1] == 0xD8) {
                    ext = "jpg";
                } else if (cover_size >= 4 && cover_data[0] == 0x89 && cover_data[1] == 0x50 && 
                           cover_data[2] == 0x4E && cover_data[3] == 0x47) {
                    ext = "png";
                }
                
                // 文件名
                static char filename[32];
                snprintf(filename, sizeof(filename), "music_cover.%s", ext);
                
                // 写入临时文件（/tmp 目录）
                char temp_path[256];
                snprintf(temp_path, sizeof(temp_path), "/tmp/%s", filename);
                
                FILE *fp = fopen(temp_path, "wb");
                if (fp) {
                    fwrite(cover_data, 1, cover_size, fp);
                    fclose(fp);
                    
                    // 使用 POSIX 文件系统路径（P: 映射到 /tmp）
                    char lvgl_path[32];
                    snprintf(lvgl_path, sizeof(lvgl_path), "P:%s", filename);
                    lv_img_set_src(ctx->ui->screen_music_img_music, lvgl_path);
                    lv_obj_invalidate(ctx->ui->screen_music_img_music);
                    lv_refr_now(NULL);
                    LOGD("custom_media_update_song_info: using song cover from temp file: %s\n", temp_path);
                } else {
                    lv_img_set_src(ctx->ui->screen_music_img_music, &_1725420839434_alpha_250x250);
                    lv_obj_invalidate(ctx->ui->screen_music_img_music);
                    LOGD("custom_media_update_song_info: failed to create temp cover file\n");
                }
            } else {
                lv_img_set_src(ctx->ui->screen_music_img_music, &_1725420839434_alpha_250x250);
                lv_obj_invalidate(ctx->ui->screen_music_img_music);
                LOGD("custom_media_update_song_info: using default cover (no cover data)\n");
            }
    } else {
        LOGD("custom_media_update_song_info: screen_music_img_music is NULL\n");
    }
    
    // 更新歌曲名显示（优先使用标题，若无则使用专辑名，最后使用文件名）
    const char *song_title = custom_media_get_song_title(idx);
    LOGD("custom_media_update_song_info: audio_title='%s'\n", song_title ? song_title : "NULL");
    if (!song_title || song_title[0] == '\0') {
        song_title = custom_media_get_song_album(idx);
        LOGD("custom_media_update_song_info: using album='%s'\n", song_title ? song_title : "NULL");
    }
    if (!song_title || song_title[0] == '\0') {
        song_title = custom_media_get_file_name(idx);
        LOGD("custom_media_update_song_info: using file_name='%s'\n", song_title ? song_title : "NULL");
    }
    if (ctx->ui->screen_music_label_song) {
        LOGD("custom_media_update_song_info: updating label_song with '%s'\n", song_title && song_title[0] != '\0' ? song_title : "未知歌曲");
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_song, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_song, song_title && song_title[0] != '\0' ? song_title : "未知歌曲");
    } else {
        LOGD("custom_media_update_song_info: screen_music_label_song is NULL\n");
    }
    
    // 更新歌手显示（若无则显示"未知歌手"）
    const char *song_artist = custom_media_get_song_artist(idx);
    LOGD("custom_media_update_song_info: audio_artist='%s'\n", song_artist ? song_artist : "NULL");
    if (ctx->ui->screen_music_label_singer) {
        LOGD("custom_media_update_song_info: updating label_singer with '%s'\n", song_artist && song_artist[0] != '\0' ? song_artist : "未知歌手");
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_singer, custom_font_get(), LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_singer, song_artist && song_artist[0] != '\0' ? song_artist : "未知歌手");
    } else {
        LOGD("custom_media_update_song_info: screen_music_label_singer is NULL\n");
    }
    
    // 更新歌词显示
    const char *lyric = custom_media_get_current_lyric();
    if (ctx->ui->screen_music_label_lyric) {
        // 设置字体（使用歌词专用字体）
        lv_obj_set_style_text_font(ctx->ui->screen_music_label_lyric, custom_font_get_lyric(), LV_PART_MAIN);
        // 启用彩色文本支持
        lv_label_set_recolor(ctx->ui->screen_music_label_lyric, true);
        // 设置自动换行和居中
        lv_label_set_long_mode(ctx->ui->screen_music_label_lyric, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(ctx->ui->screen_music_label_lyric, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_label_set_text(ctx->ui->screen_music_label_lyric, lyric ? lyric : "暂无歌词");
    }
}

/**
 * @brief 进度更新回调函数
 */
static void progress_update_callback(lv_timer_t *timer) {
    MediaContext *ctx = get_context();
    if (!ctx->ui || !ctx->pc) return;
    
    int state = custom_media_get_state();
    if (state != 1) return;  // 只在播放时更新
    
    // 更新当前时间
    double pos = custom_media_get_position();
    char pos_str[16];
    custom_media_format_duration((int64_t)(pos * 1000), pos_str, sizeof(pos_str));
    lv_label_set_text(ctx->ui->screen_music_label_progressTime, pos_str);
    
    
    // 更新总时长
    double duration = custom_media_get_duration();
    char dur_str[16];
    custom_media_format_duration((int64_t)(duration * 1000), dur_str, sizeof(dur_str));
    lv_label_set_text(ctx->ui->screen_music_label_endTime, dur_str);
    
    // 更新进度条
    if (duration > 0) {
        int progress = (int)((pos / duration) * 100);
        if (ctx->ui->screen_music_slider_progressTime) {
            lv_slider_set_value(ctx->ui->screen_music_slider_progressTime, progress, LV_ANIM_OFF);
        }
    }
    
    // 更新歌词
    custom_media_update_lyric_index(pos);
}

/**
 * @brief 开始进度更新（播放时调用）
 */
void custom_media_start_progress_update(void) {
    MediaContext *ctx = get_context();
    if (ctx->progress_timer) {
        lv_timer_resume(ctx->progress_timer);
    }
}

/**
 * @brief 停止进度更新（暂停/停止时调用）
 */
void custom_media_stop_progress_update(void) {
    MediaContext *ctx = get_context();
    if (ctx->progress_timer) {
        lv_timer_pause(ctx->progress_timer);
    }
}



/**
 * @brief 反初始化媒体播放核心模块，释放所有资源
 */
void custom_media_deinit(void) {
    MediaContext *ctx = get_context();
    
    LOGD("custom_media_deinit: 开始反初始化\n");
    
    // 停止定时器（先停止定时器，防止回调访问已释放资源）
    // LVGL定时器是线程安全的，可以直接删除
    LOGD("custom_media_deinit: 停止进度定时器\n");
    if (ctx->progress_timer) {
        lv_timer_del(ctx->progress_timer);
        ctx->progress_timer = NULL;
        LOGD("custom_media_deinit: 进度定时器已停止\n");
    }
    
    LOGD("custom_media_deinit: 停止视频帧定时器, timer=%p\n", ctx->video_frame_timer);
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        LOGD("custom_media_deinit: 视频帧定时器已停止\n");
    } else {
        LOGD("custom_media_deinit: 视频帧定时器为NULL\n");
    }
    
    // 清空视频图像控件内容（防止屏幕切换时访问已释放的帧缓冲区）
    // 只有在视频模式下才处理视频控件
    if (ctx->play_mode == 1 && ctx->ui && ctx->ui->screen_video_img_video) {
        LOGD("custom_media_deinit: 清空视频图像控件\n");
        lv_img_set_src(ctx->ui->screen_video_img_video, LV_SYMBOL_OK); // 使用内置图标代替空字符串
        lv_obj_invalidate(ctx->ui->screen_video_img_video);
    } else {
        LOGD("custom_media_deinit: 非视频模式，跳过视频控件处理\n");
    }
    
    // 等待视频解码器线程完全退出
    usleep(30000); // 30ms
    
    // 等待定时器回调完成（给正在执行的回调时间退出）
    LOGD("custom_media_deinit: 等待定时器回调完成...\n");
    
    LOGD("custom_media_deinit: 定时器等待完成\n");
    
    // 停止播放器（确保解码器线程退出）
    // 注意：player_core_destroy() 内部已经调用了 player_core_stop()，不需要重复调用
    if (ctx->pc) {
        player_core_destroy(ctx->pc);
        ctx->pc = NULL;
    }
    
    // 释放歌词内存
    for (int i = 0; i < ctx->lyrics_count; i++) {
        free(ctx->lyrics[i]);
        ctx->lyrics[i] = NULL;
    }
    ctx->lyrics_count = 0;
    ctx->current_lyric_idx = -1;
    
    // 销毁播放列表
    if (ctx->audio_playlist) {
        playlist_destroy(ctx->audio_playlist);
        ctx->audio_playlist = NULL;
    }
    if (ctx->video_playlist) {
        playlist_destroy(ctx->video_playlist);
        ctx->video_playlist = NULL;
    }
    
    // 重置 UI 指针
    ctx->ui = NULL;
    
    // 等待所有操作完成
    usleep(100000); // 100ms
    
    LOGD("custom_media_deinit: done\n");
}

/**
 * @brief 播放音频文件
 * @param file 音频文件路径
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_audio(const char *file) {
    MediaContext *ctx = get_context();
    
    if (!file || !ctx->pc) return -1;
    
    int play_mode = custom_media_get_play_mode();
    LOGD("custom_media_play_audio: 播放音频 %s, 播放模式=%d\n", file, play_mode);
    
    // 如果是视频模式，不允许播放音频
    if (play_mode == 1) {
        LOGD("custom_media_play_audio: 错误！视频模式下不能播放音频！\n");
        return -1;
    }
    
    // 先暂停当前播放
    if (custom_media_get_state() == 1) {
        custom_media_pause();
        custom_media_stop_progress_update();
    }
    
    // 查找文件在列表中的索引
    int idx = -1;
    if (ctx->audio_playlist) {
        for (int i = 0; i < playlist_get_count(ctx->audio_playlist); i++) {
            PlaylistItem *item = playlist_get_item(ctx->audio_playlist, i);
            if (item && strcmp(item->file_path, file) == 0) {
                idx = i;
                playlist_set_current_idx(ctx->audio_playlist, i);
                break;
            }
        }
    }
    
    // 播放音频
    int result = player_core_play_audio(ctx->pc, file);
    
    // 更新UI
    if (ctx->ui && idx >= 0) {
        custom_media_update_ui_display();
    }
    
    return result;
}

/**
 * @brief 播放指定索引的歌曲
 * @param idx 歌曲索引
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_song_by_index(int idx) {
    MediaContext *ctx = get_context();
    
    if (!ctx->audio_playlist || !ctx->pc) return -1;
    
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item) return -1;
    
    // 先暂停当前播放
    if (custom_media_get_state() == 1) {
        custom_media_pause();
        custom_media_stop_progress_update();
    }
    
    // 设置当前索引
    playlist_set_current_idx(ctx->audio_playlist, idx);
    
    // 播放指定索引的歌曲
    int result = player_core_play_audio(ctx->pc, item->file_path);
    
    // 打印当前播放歌曲的元数据信息
    if (result == 0) {
        LOGD("custom_media_play_song_by_index: ====== Playing Song %d ======\n", idx);
        LOGD("custom_media_play_song_by_index: file: %s\n", item->file_path);
        LOGD("custom_media_play_song_by_index: title: %s\n", item->title ? item->title : "(null)");
        LOGD("custom_media_play_song_by_index: artist: %s\n", item->artist ? item->artist : "(null)");
        LOGD("custom_media_play_song_by_index: album: %s\n", item->album ? item->album : "(null)");
        LOGD("custom_media_play_song_by_index: duration: %.2fs\n", item->duration_ms / 1000.0);
        
        // 打印歌词信息
        char lrc_path[512];
        const char *ext = strrchr(item->file_path, '.');
        if (ext) {
            int basename_len = ext - item->file_path;
            snprintf(lrc_path, sizeof(lrc_path), "%.*s.lrc", basename_len, item->file_path);
        } else {
            snprintf(lrc_path, sizeof(lrc_path), "%s.lrc", item->file_path);
        }
        FILE *fp = fopen(lrc_path, "r");
        if (fp) {
            LOGD("custom_media_play_song_by_index: lyrics: found (%s)\n", lrc_path);
            fclose(fp);
        } else {
            LOGD("custom_media_play_song_by_index: lyrics: not found (%s)\n", lrc_path);
        }
        LOGD("custom_media_play_song_by_index: ==============================\n");
    }
    
    // 更新UI
    if (ctx->ui) {
        // 解析歌词
        custom_media_parse_lyrics(item->file_path);
        custom_media_update_ui_display();
    }
    
    return result;
}

/**
 * @brief 播放下一首
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_next(void) {
    MediaContext *ctx = get_context();
    
    if (!ctx->audio_playlist || !ctx->pc) return -1;
    
    int count = playlist_get_count(ctx->audio_playlist);
    if (count <= 0) return -1;
    
    // 先暂停当前播放
    if (custom_media_get_state() == 1) {
        custom_media_pause();
        custom_media_stop_progress_update();
    }
    
    // 播放下一首
    int result = player_core_play_next(ctx->pc);
    
    // 更新UI
    if (ctx->ui) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            // 解析歌词
            custom_media_parse_lyrics(item->file_path);
        }
        custom_media_update_ui_display();
    }
    
    return result >= 0 ? 0 : -1;
}

/**
 * @brief 播放上一首
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_prev(void) {
    MediaContext *ctx = get_context();
    
    if (!ctx->audio_playlist || !ctx->pc) return -1;
    
    int count = playlist_get_count(ctx->audio_playlist);
    if (count <= 0) return -1;
    
    // 先暂停当前播放
    if (custom_media_get_state() == 1) {
        custom_media_pause();
        custom_media_stop_progress_update();
    }
    
    // 播放上一首
    int result = player_core_play_prev(ctx->pc);
    
    // 更新UI
    if (ctx->ui) {
        PlaylistItem *item = playlist_get_current_item(ctx->audio_playlist);
        if (item) {
            // 解析歌词
            custom_media_parse_lyrics(item->file_path);
        }
        custom_media_update_ui_display();
    }
    
    return result >= 0 ? 0 : -1;
}

/**
 * @brief 获取当前歌曲索引
 * @return 当前歌曲索引
 */
int custom_media_get_current_song_idx(void) {
    MediaContext *ctx = get_context();
    return ctx->audio_playlist ? playlist_get_current_idx(ctx->audio_playlist) : -1;
}

/**
 * @brief 获取歌曲总数
 * @return 歌曲总数
 */
int custom_media_get_song_count(void) {
    MediaContext *ctx = get_context();
    return ctx->audio_playlist ? playlist_get_count(ctx->audio_playlist) : 0;
}

/**
 * @brief 获取指定索引的歌曲路径
 * @param idx 歌曲索引
 * @return 歌曲路径，索引无效返回NULL
 */
const char* custom_media_get_song_path(int idx) {
    MediaContext *ctx = get_context();
    if (!ctx->audio_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item ? item->file_path : NULL;
}

/**
 * @brief 从歌曲路径提取文件名（不含路径和扩展名）
 * @param path 歌曲路径
 * @return 文件名（需要调用者free）
 */
char* custom_media_get_song_name(const char *path) {
    if (!path) return NULL;
    
    // 找到最后一个 '/' 的位置
    const char *slash = strrchr(path, '/');
    const char *name_start = slash ? slash + 1 : path;
    
    // 找到最后一个 '.' 的位置
    const char *dot = strrchr(name_start, '.');
    int len = dot ? (dot - name_start) : strlen(name_start);
    
    char *name = malloc(len + 1);
    if (name) {
        strncpy(name, name_start, len);
        name[len] = '\0';
    }
    return name;
}

/**
 * @brief 获取文件名（含扩展名）
 * @param idx 歌曲索引
 * @return 文件名
 */
const char* custom_media_get_file_name(int idx) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_get_file_name: idx=%d\n", idx);
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item || !item->file_path) return "";
    
    // 提取文件名
    const char *slash = strrchr(item->file_path, '/');
    return slash ? slash + 1 : item->file_path;
}

/**
 * @brief 获取歌名
 * @param idx 歌曲索引
 * @return 歌名
 */
const char* custom_media_get_song_title(int idx) {
    MediaContext *ctx = get_context();
    LOGD("custom_media_get_song_title: idx=%d\n", idx);
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->title ? item->title : "";
}

/**
 * @brief 获取歌手
 * @param idx 歌曲索引
 * @return 歌手
 */
const char* custom_media_get_song_artist(int idx) {
    MediaContext *ctx = get_context();
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->artist ? item->artist : "";
}

/**
 * @brief 获取歌曲封面数据
 * @param idx 歌曲索引
 * @param size 输出参数，返回封面数据大小
 * @return 封面数据指针，若无则返回NULL
 */
uint8_t* custom_media_get_song_cover(int idx, size_t *size) {
    MediaContext *ctx = get_context();
    if (!ctx->audio_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    if (!item || !item->cover_data || item->cover_size == 0) return NULL;
    if (size) *size = item->cover_size;
    return item->cover_data;
}

/**
 * @brief 获取专辑
 * @param idx 歌曲索引
 * @return 专辑
 */
const char* custom_media_get_song_album(int idx) {
    MediaContext *ctx = get_context();
    if (!ctx->audio_playlist) return "";
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item && item->album ? item->album : "";
}

/**
 * @brief 获取歌曲时长（毫秒）
 * @param idx 歌曲索引
 * @return 时长（毫秒）
 */
int64_t custom_media_get_song_duration_ms(int idx) {
    MediaContext *ctx = get_context();
    if (!ctx->audio_playlist) return 0;
    PlaylistItem *item = playlist_get_item(ctx->audio_playlist, idx);
    return item ? item->duration_ms : 0;
}

/**
 * @brief 格式化时长为 MM:SS
 * @param duration_ms 时长（毫秒）
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 */
void custom_media_format_duration(int64_t duration_ms, char *buf, size_t buf_size) {
    media_metadata_format_duration_short(duration_ms, buf, buf_size);
}

/**
 * @brief 播放视频文件
 * @param file 视频文件路径
 * @param width 目标显示宽度
 * @param height 目标显示高度
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_video(const char *file, int width, int height) {
    MediaContext *ctx = get_context();
    return ctx->pc ? player_core_play_video(ctx->pc, file, width, height) : -1;
}

/**
 * @brief 暂停当前播放
 */
void custom_media_pause(void) {
    MediaContext *ctx = get_context();
    if (ctx->pc) {
        int play_mode = custom_media_get_play_mode();
        LOGD("custom_media_pause: 播放模式=%d\n", play_mode);
        player_core_pause(ctx->pc);
    }
}

/**
 * @brief 恢复暂停的播放
 */
void custom_media_resume(void) {
    MediaContext *ctx = get_context();
    if (ctx->pc) {
        int play_mode = custom_media_get_play_mode();
        int state = custom_media_get_state();
        LOGD("custom_media_resume: 状态=%d, 播放模式=%d\n", state, play_mode);
        
        // 如果是空闲状态且是视频模式，不执行任何操作（防止自动播放）
        if (state == 0 && play_mode == 1) {
            LOGD("custom_media_resume: 视频模式下空闲状态，忽略\n");
            return;
        }
        
        player_core_resume(ctx->pc);
    }
}

/**
 * @brief 设置播放模式
 * @param mode 0-音乐模式，1-视频模式
 */
void custom_media_set_play_mode(int mode) {
    MediaContext *ctx = get_context();
    ctx->play_mode = mode;
}

/**
 * @brief 获取当前播放模式
 * @return 0-音乐模式，1-视频模式
 */
int custom_media_get_play_mode(void) {
    MediaContext *ctx = get_context();
    return ctx->play_mode;
}

/**
 * @brief 停止当前播放
 */
void custom_media_stop(void) {
    MediaContext *ctx = get_context();
    
    // 停止视频帧定时器（如果存在）
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
    }
    
    // 停止播放器核心
    if (ctx->pc) player_core_stop(ctx->pc);
}

/**
 * @brief 跳转到指定时间位置（秒）
 * @param sec 目标时间（单位：秒）
 */
void custom_media_seek(double sec) {
    MediaContext *ctx = get_context();
    if (ctx->pc) player_core_seek(ctx->pc, sec);
}

/**
 * @brief 获取当前播放位置（秒）
 * @return 当前播放时间（秒）
 */
double custom_media_get_position(void) {
    MediaContext *ctx = get_context();
    return ctx->pc ? player_core_get_position(ctx->pc) : 0.0;
}

/**
 * @brief 获取媒体总时长（秒）
 * @return 媒体总时长（秒）
 */
double custom_media_get_duration(void) {
    MediaContext *ctx = get_context();
    return ctx->pc ? player_core_get_duration(ctx->pc) : 0.0;
}

/**
 * @brief 获取当前播放状态
 * @return 0:idle, 1:playing, 2:paused
 */
int custom_media_get_state(void) {
    MediaContext *ctx = get_context();
    return ctx->pc ? player_core_get_state(ctx->pc) : 0;
}

/**
 * @brief 设置音量
 * @param volume 音量值（0.0 - 2.0，1.0为原始音量）
 */
void custom_media_set_volume(float volume) {
    MediaContext *ctx = get_context();
    if (ctx->pc) {
        player_core_set_volume(ctx->pc, volume);
    }
}

/**
 * @brief 获取当前音量
 * @return 当前音量值
 */
float custom_media_get_volume(void) {
    MediaContext *ctx = get_context();
    return ctx->pc ? player_core_get_volume(ctx->pc) : 1.0f;
}

/**
 * @brief 获取当前就绪的视频帧数据（RGB565格式）
 * @return 视频帧缓冲区指针，若无则返回NULL
 */
uint8_t* custom_media_get_video_frame(void) {
    MediaContext *ctx = get_context();
    if (!ctx->pc) return NULL;
    return player_core_get_video_frame(ctx->pc);
}

/**
 * @brief 释放已获取的视频帧，允许解码器写入新帧
 */
void custom_media_release_video_frame(void) {
    MediaContext *ctx = get_context();
    if (ctx->pc) {
        // 额外检查播放器状态
        if (custom_media_get_state() == 1 && custom_media_get_play_mode() == 1) {
            player_core_release_video_frame(ctx->pc);
        }
    }
}

/**
 * @brief 更新视频帧显示（供 LVGL 定时器调用）
 * @param timer LVGL 定时器指针（未使用）
 */
void custom_media_update_video_frame(lv_timer_t *timer) {
    MediaContext *ctx = get_context();
    
    // 检查上下文是否有效
    if (!ctx) return;
    
    // 检查播放器核心是否存在（最重要：防止访问已释放的资源）
    if (!ctx->pc) return;
    
    // 检查 UI 是否有效
    if (!ctx->ui || !ctx->ui->screen_video_img_video) return;
    
    // 检查是否正在播放
    if (custom_media_get_state() != 1) return;
    
    // 在访问播放器核心之前再次检查（防止竞态条件）
    if (!ctx->pc) return;
    
    uint8_t *frame = custom_media_get_video_frame();
    if (frame) {
        // 创建图像描述符显示RGB565格式视频帧
        static lv_img_dsc_t video_img_dsc = {
            .header.always_zero = 0,
            .header.w = VIDEO_WIDTH,
            .header.h = VIDEO_HEIGHT,
            .header.cf = LV_IMG_CF_TRUE_COLOR, // RGB565 格式
            .data_size = VIDEO_WIDTH * VIDEO_HEIGHT * 2,
            .data = NULL
        };
        // 使用常量视频分辨率
        video_img_dsc.header.w = VIDEO_WIDTH;
        video_img_dsc.header.h = VIDEO_HEIGHT;
        video_img_dsc.data_size = VIDEO_WIDTH * VIDEO_HEIGHT * 2;
        video_img_dsc.data = frame;
        
        // 再次检查 UI（防止在设置过程中 UI 被销毁）
        if (!ctx->ui || !ctx->ui->screen_video_img_video) return;
        
        // 将帧数据设置到视频图像控件
        lv_img_set_src(ctx->ui->screen_video_img_video, &video_img_dsc);
        
        // 设置视频居中显示
        lv_obj_center(ctx->ui->screen_video_img_video);
        
        lv_obj_invalidate(ctx->ui->screen_video_img_video);
    }
    
    // 在释放缓冲区之前再次检查播放器核心
    if (!ctx->pc) return;
    
    // 无论是否获取到帧，都需要释放缓冲区（重置ui_busy标志）
    // 这是关键：确保解码器可以继续写入新帧
    custom_media_release_video_frame();
    
    // 在更新进度之前再次检查
    if (!ctx->pc || !ctx->ui) return;
    
    // 同时更新进度条和时间显示
    custom_media_update_video_progress();
    custom_media_update_video_time_display();
}

/**
 * @brief 更新视频进度条
 */
void custom_media_update_video_progress(void) {
    MediaContext *ctx = get_context();
    if (!ctx->ui || !ctx->ui->screen_video_bar_time) return;
    
    double duration = custom_media_get_duration();
    double position = custom_media_get_position();
    
    if (duration > 0) {
        int progress = (int)(position / duration * 100);
        lv_bar_set_value(ctx->ui->screen_video_bar_time, progress, LV_ANIM_OFF);
    }
}

/**
 * @brief 更新视频时间显示
 */
void custom_media_update_video_time_display(void) {
    MediaContext *ctx = get_context();
    if (!ctx->ui || !ctx->ui->screen_video_label_progressTime) return;
    
    double duration = custom_media_get_duration();
    double position = custom_media_get_position();
    
    char time_str[32];
    int pos_min = (int)(position / 60);
    int pos_sec = (int)(position - pos_min * 60);
    int dur_min = (int)(duration / 60);
    int dur_sec = (int)(duration - dur_min * 60);
    
    snprintf(time_str, sizeof(time_str), "%02d:%02d/%02d:%02d", 
             pos_min, pos_sec, dur_min, dur_sec);
    lv_label_set_text(ctx->ui->screen_video_label_progressTime, time_str);
}

/**
 * @brief 更新视频文件名显示
 * @param file_name 文件名
 */
void custom_media_update_video_filename(const char *file_name) {
    MediaContext *ctx = get_context();
    if (!ctx->ui || !ctx->ui->screen_video_label_fileName || !file_name) return;
    
    // 提取文件名（去掉路径）
    const char *name = strrchr(file_name, '/');
    if (name) name++;
    else name = file_name;
    
    // 设置字体（使用freetype字体避免乱码）
    lv_obj_set_style_text_font(ctx->ui->screen_video_label_fileName, custom_font_get(), LV_PART_MAIN);
    lv_label_set_text(ctx->ui->screen_video_label_fileName, name);
}

// ========== 视频播放列表函数 ==========

/**
 * @brief 播放指定索引的视频
 * @param idx 视频索引
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_video_by_index(int idx) {
    MediaContext *ctx = get_context();
    
    LOGD("custom_media_play_video_by_index: 调用, idx=%d\n", idx);
    
    if (!ctx->video_playlist) {
        LOGD("custom_media_play_video_by_index: video_playlist 为空\n");
        return -1;
    }
    if (!ctx->pc) {
        LOGD("custom_media_play_video_by_index: pc 为空\n");
        return -1;
    }
    
    PlaylistItem *item = playlist_get_item(ctx->video_playlist, idx);
    if (!item) {
        LOGD("custom_media_play_video_by_index: item 为空\n");
        return -1;
    }
    
    LOGD("custom_media_play_video_by_index: 播放视频 %s\n", item->file_path);
    
    // 停止视频帧更新定时器
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
    }
    
    // 等待一小段时间，确保LVGL定时器线程完成当前回调
    usleep(50000);  // 50ms
    
    // 设置当前索引
    playlist_set_current_idx(ctx->video_playlist, idx);
    
    // 更新文件名显示
    custom_media_update_video_filename(item->file_path);
    
    // 动态调整图像控件大小
    if (ctx->ui && ctx->ui->screen_video_img_video) {
        lv_obj_set_size(ctx->ui->screen_video_img_video, VIDEO_WIDTH, VIDEO_HEIGHT);
        lv_obj_center(ctx->ui->screen_video_img_video);
        LOGD("custom_media_play_video_by_index: 图像控件大小设置为 %dx%d\n", VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    
    // 播放视频
    int result = player_core_play_video(ctx->pc, item->file_path, VIDEO_WIDTH, VIDEO_HEIGHT);
    LOGD("custom_media_play_video_by_index: 结果=%d\n", result);
    
    if (result != 0) {
        // 视频播放失败，显示错误提示
        LOGD("custom_media_play_video_by_index: 视频播放失败，可能是格式不支持\n");
        if (ctx->ui && ctx->ui->screen_video_label_fileName) {
            lv_label_set_text(ctx->ui->screen_video_label_fileName, "视频格式不支持");
        }
        // 停止视频帧更新定时器（如果还在运行）
        if (ctx->video_frame_timer) {
            lv_timer_del(ctx->video_frame_timer);
            ctx->video_frame_timer = NULL;
        }
        return -1;
    }
    
    // 启动视频帧更新定时器（根据视频帧率动态设置）
    int frame_interval_ms = (int)(1000.0 / 24.0); // 使用视频原帧率24fps
    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, frame_interval_ms, NULL);
    
    // 确保画面可见（防止切换后黑屏）
    if (ctx->ui && ctx->ui->screen_video_img_video) {
        // 短暂延迟后强制刷新画面
        usleep(100000); // 等待100ms让第一帧准备好
        lv_obj_invalidate(ctx->ui->screen_video_img_video);
    }
    
    return result;
}

/**
 * @brief 播放下一个视频
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_next_video(void) {
    MediaContext *ctx = get_context();
    if (!ctx->video_playlist || !ctx->pc) return -1;
    
    int count = playlist_get_count(ctx->video_playlist);
    if (count <= 0) return -1;
    
    // 停止视频帧更新定时器
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        // 短暂等待，确保定时器完全停止
        usleep(50000);
    }
    
    // 播放下一个
    int result = player_core_play_next(ctx->pc);
    
    // 重新启动视频帧更新定时器
    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, 33, NULL);
    
    return result >= 0 ? 0 : -1;
}

/**
 * @brief 播放上一个视频
 * @return 成功返回0，失败返回-1
 */
int custom_media_play_prev_video(void) {
    MediaContext *ctx = get_context();
    if (!ctx->video_playlist || !ctx->pc) return -1;
    
    int count = playlist_get_count(ctx->video_playlist);
    if (count <= 0) return -1;
    
    // 停止视频帧更新定时器
    if (ctx->video_frame_timer) {
        lv_timer_del(ctx->video_frame_timer);
        ctx->video_frame_timer = NULL;
        // 短暂等待，确保定时器完全停止
        usleep(50000);
    }
    
    // 播放上一个
    int result = player_core_play_prev(ctx->pc);
    
    // 重新启动视频帧更新定时器
    ctx->video_frame_timer = lv_timer_create(custom_media_update_video_frame, 33, NULL);
    
    return result >= 0 ? 0 : -1;
}

/**
 * @brief 获取当前视频索引
 * @return 当前视频索引
 */
int custom_media_get_current_video_idx(void) {
    MediaContext *ctx = get_context();
    return ctx->video_playlist ? playlist_get_current_idx(ctx->video_playlist) : -1;
}

/**
 * @brief 获取视频总数
 * @return 视频总数
 */
int custom_media_get_video_count(void) {
    MediaContext *ctx = get_context();
    return ctx->video_playlist ? playlist_get_count(ctx->video_playlist) : 0;
}

/**
 * @brief 获取指定索引的视频路径
 * @param idx 视频索引
 * @return 视频路径，索引无效返回NULL
 */
const char* custom_media_get_video_path(int idx) {
    MediaContext *ctx = get_context();
    if (!ctx->video_playlist) return NULL;
    PlaylistItem *item = playlist_get_item(ctx->video_playlist, idx);
    return item ? item->file_path : NULL;
}

// ========== 获取播放列表指针 ==========

/**
 * @brief 获取音频播放列表指针
 * @return 音频播放列表指针
 */
Playlist* custom_media_get_audio_playlist(void) {
    MediaContext *ctx = get_context();
    return ctx->audio_playlist;
}

/**
 * @brief 获取视频播放列表指针
 * @return 视频播放列表指针
 */
Playlist* custom_media_get_video_playlist(void) {
    MediaContext *ctx = get_context();
    return ctx->video_playlist;
}

// ========== 列表 UI 实现 ==========

static void music_list_item_click_event(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    LOGD("music_list_item_click_event: clicked item %d\n", idx);
    
    // 播放歌曲
    custom_media_play_song_by_index(idx);
    
    // 启动进度更新
    custom_media_start_progress_update();
    
    // 更新UI显示
    custom_media_update_ui_display();
}

static void video_list_item_click_event(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    LOGD("video_list_item_click_event: clicked item %d\n", idx);
    
    // 播放视频
    int result = custom_media_play_video_by_index(idx);
    
    // 更新播放按钮状态图标（从播放状态改为暂停状态）
    MediaContext *ctx = get_context();
    if (result == 0 && ctx->ui && ctx->ui->screen_video_btn_playing_label) {
        lv_label_set_text(ctx->ui->screen_video_btn_playing_label, LV_SYMBOL_PAUSE);
        LOGD("video_list_item_click_event: 更新播放按钮为暂停图标\n");
    }
}

void custom_media_init_music_list(lv_ui *ui) {
    MediaContext *ctx = get_context();
    
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
    
    // 清空现有列表项
    lv_obj_clean(ui->screen_music_list_music);
    
    int count = playlist_get_count(ctx->audio_playlist);
    LOGD("custom_media_init_music_list: 找到 %d 首歌曲\n", count);
    
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
        
        // 添加列表项
        lv_obj_t *btn = lv_list_add_btn(ui->screen_music_list_music, LV_SYMBOL_AUDIO, display_name);
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        
        // 应用原有样式
        static lv_style_t style_list_btn;
        if (i == 0) {
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
        }
        lv_obj_add_style(btn, &style_list_btn, LV_PART_MAIN|LV_STATE_DEFAULT);
        
        // 添加点击事件
        lv_obj_add_event_cb(btn, music_list_item_click_event, LV_EVENT_CLICKED, NULL);
    }
    
    LOGD("custom_media_init_video_list: 完成, 状态=%d, 播放模式=%d\n", custom_media_get_state(), custom_media_get_play_mode());
    
    // 立即检查是否有音乐在播放
    if (custom_media_get_state() == 1) {
        LOGD("custom_media_init_video_list: 警告！音乐正在播放，停止中...\n");
        custom_media_stop();
        LOGD("custom_media_init_video_list: 停止后, 状态=%d\n", custom_media_get_state());
    }
    
    // 延迟检查已移至视频帧更新定时器中处理
}

void custom_media_init_video_list(lv_ui *ui) {
    MediaContext *ctx = get_context();
    
    LOGD("custom_media_init_video_list: 调用, 状态=%d, 播放模式=%d\n", custom_media_get_state(), custom_media_get_play_mode());
    
    if (!ui || !ui->screen_video_list_video) {
        LOGD("custom_media_init_video_list: ui 或 screen_video_list_video 为空\n");
        return;
    }
    
    if (!ctx->video_playlist) {
        LOGD("custom_media_init_video_list: video_playlist 为空\n");
        return;
    }
    
    // 确保没有音乐在播放
    if (custom_media_get_state() == 1) {
        LOGD("custom_media_init_video_list: stopping audio before init\n");
        custom_media_stop();
    }
    
    LOGD("custom_media_init_video_list: 初始化视频列表\n");
    lv_obj_clean(ui->screen_video_list_video);
    
    int count = playlist_get_count(ctx->video_playlist);
    LOGD("custom_media_init_video_list: 找到 %d 个视频\n", count);
    
    // 定义列表项样式（与音乐列表保持一致）
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
    lv_style_set_text_font(&style_list_btn, custom_font_get()); // 使用freetype字体
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
        
        // 添加列表项，使用LV_SYMBOL_VIDEO图标
        lv_obj_t *btn = lv_list_add_btn(ui->screen_video_list_video, LV_SYMBOL_VIDEO, display_name);
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        
        // 应用样式（与音乐列表保持一致）
        lv_obj_add_style(btn, &style_list_btn, LV_PART_MAIN|LV_STATE_DEFAULT);
        
        lv_obj_add_event_cb(btn, video_list_item_click_event, LV_EVENT_CLICKED, NULL);
    }
    
    // 刷新列表组件
    lv_obj_invalidate(ui->screen_video_list_video);
    
    LOGD("custom_media_init_video_list: 完成\n");
}