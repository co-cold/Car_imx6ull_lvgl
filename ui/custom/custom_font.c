#include "custom_font.h"

static lv_ft_info_t ft_info_normal;  // 普通字体（16号）
static lv_ft_info_t ft_info_lyric;   // 歌词字体（18号）

const lv_font_t* custom_font_init(const char *font_path, uint32_t font_size) {
    // 初始化 FreeType（与示例代码一致）
    lv_freetype_init(8, 16, 256 * 1024);
    
    // 初始化普通字体（16号）
    ft_info_normal.name = font_path;
    ft_info_normal.weight = font_size;  // 字体大小作为 weight 参数
    ft_info_normal.style = FT_FONT_STYLE_NORMAL;
    ft_info_normal.mem = NULL;
    
    if (!lv_ft_font_init(&ft_info_normal)) {
        LOGE("custom_font: failed to open font %s\n", font_path);
        return NULL;
    }
    
    // 初始化歌词字体（18号，比普通字体大2）
    ft_info_lyric.name = font_path;
    ft_info_lyric.weight = font_size + 2;  // 歌词字体大2号
    ft_info_lyric.style = FT_FONT_STYLE_NORMAL;
    ft_info_lyric.mem = NULL;
    
    if (!lv_ft_font_init(&ft_info_lyric)) {
        LOGE("custom_font: failed to open lyric font %s\n", font_path);
    }
    
    LOGD("custom_font: loaded font %s, normal size %u, lyric size %u\n", 
         font_path, font_size, font_size + 2);
    return ft_info_normal.font;
}

const lv_font_t* custom_font_get(void) {
    // 如果字体未初始化，返回 NULL，LVGL 将使用默认字体
    if (!ft_info_normal.font) {
        LOGD("custom_font_get: font not initialized, returning NULL\n");
        return NULL;
    }
    return ft_info_normal.font;
}

const lv_font_t* custom_font_get_lyric(void) {
    // 如果歌词字体未初始化，返回普通字体
    if (!ft_info_lyric.font) {
        LOGD("custom_font_get_lyric: lyric font not initialized, returning normal font\n");
        return custom_font_get();
    }
    return ft_info_lyric.font;
}