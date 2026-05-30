#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "gui_guider.h"
#include "events_init.h"                // 包含事件初始化相关函数声明
#include "custom.h"                     // 包含自定义功能相关函数声明

lv_ui guider_ui;                        // 声明GUI Guider生成的UI结构体实例

#define DISP_BUF_SIZE (800 * 480)

int lvgl_init()
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[2][DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf[0], buf[1], DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
    // LV_IMG_DECLARE(mouse_cursor_icon)
    // lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    // lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    // lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
}

int main(int argc, char *argv[])
{
    lvgl_init();

    /* Initialize extra libraries (PNG, JPEG, etc.) */
    lv_extra_init();
    printf("LVGL extra libraries initialized\n");
    
    // 检查 PNG 解码是否可用
    #if LV_USE_PNG
    printf("PNG decoder: enabled\n");
    #else
    printf("PNG decoder: disabled\n");
    #endif
    
    // 检查 JPEG 解码是否可用
    #if LV_USE_SJPG
    printf("JPEG decoder: enabled\n");
    #else
    printf("JPEG decoder: disabled\n");
    #endif
    
    //lv_freetype_init(8, 16, 256 * 1024);
    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_music();

    setup_ui(&guider_ui);               // 调用GUI Guider生成的UI设置函数，初始化界面
    events_init(&guider_ui);            // 初始化事件处理函数，绑定UI元素与事件
    custom_init(&guider_ui);            // 初始化自定义功能
    printf("bug\n");
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}