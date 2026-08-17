#include "events_init_common.h"

bool is_music_list = false;

void music_list_show(lv_ui *ui)
{
    is_music_list = true;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui->screen_music_cont_list);
    lv_anim_set_values(&a, 480, 180);
    lv_anim_set_time(&a, 300);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void music_list_hide(lv_ui *ui)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui->screen_music_cont_list);
    lv_anim_set_values(&a, 180, 480);
    lv_anim_set_time(&a, 300);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_start(&a);
    is_music_list = false;
}