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

#include "gui_guider.h"
#include "events_init.h"                // 包含事件初始化相关函数声明
#include "custom.h"                     // 包含自定义功能相关函数声明

// 压力测试相关
#include "multimedia/player_core.h"
#define TEST_AUDIO_FILE "/home/debian/music/GET IN THE RING - Thank you for dears.flac"
#define TEST_VIDEO_FILE "/home/debian/video/犬夜叉片头.avi"
#define TEST_DURATION 3 // 每次播放的测试时长（秒）
#define TEST_LOOPS 5    // 测试循环次数

lv_ui guider_ui;                        // 声明GUI Guider生成的UI结构体实例

#define DISP_BUF_SIZE (800 * 480)

int lvgl_init()
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    disp_drv.full_refresh = 0;  // 部分刷新
    disp_drv.direct_mode = 0;   // 使用绘制缓冲区
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

// 模拟视频播放后切换音乐再退出的场景（崩溃场景）
int test_video_to_music_exit(PlayerCore *pc) {
    printf("[TEST] Video->Music->Exit...");
    
    // 播放视频
    if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
        printf(" [FAILED]\n");
        return -1;
    }
    usleep(100000);
    sleep(2);
    
    // 切换到音乐模式
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf(" [FAILED]\n");
        return -1;
    }
    sleep(2);
    
    // 停止播放
    player_core_stop(pc);
    usleep(30000);
    usleep(100000);
    
    printf(" [PASSED]\n");
    return 0;
}

// 模拟快速切换测试
int test_quick_switch(PlayerCore *pc) {
    printf("[TEST] Quick switch...");
    
    for (int i = 0; i < 3; i++) {
        if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
            printf(" [FAILED]\n");
            return -1;
        }
        usleep(500000);
        
        if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
            printf(" [FAILED]\n");
            return -1;
        }
        usleep(500000);
    }
    
    player_core_stop(pc);
    printf(" [PASSED]\n");
    return 0;
}

// 压力测试函数
int run_stress_test(void) {
    printf("=========================================\n");
    printf("Multimedia Module Stress Test\n");
    printf("Test scenarios simulating custom_media interaction\n");
    printf("=========================================\n");
    
    int failed_tests = 0;
    int total_tests = 0;
    
    // 运行多个测试循环
    for (int loop = 0; loop < TEST_LOOPS; loop++) {
        printf("\n--- Test Loop %d/%d ---\n", loop + 1, TEST_LOOPS);
        
        // 创建播放器核心
        PlayerCore *pc = player_core_create();
        if (!pc) {
            printf("[ERROR] Failed to create player core\n");
            return -1;
        }
        
        // 测试1: 视频->音乐->退出（崩溃场景）
        total_tests++;
        printf("\n--- Test 1: Video->Music->Exit ---");
        if (test_video_to_music_exit(pc) != 0) {
            failed_tests++;
        }
        
        // 测试2: 快速切换
        total_tests++;
        printf("--- Test 2: Quick Switch ---");
        if (test_quick_switch(pc) != 0) {
            failed_tests++;
        }
        
        // 测试3: 模式切换
        total_tests++;
        printf("--- Test 3: Mode Switch ---\n");
        printf("[TEST] Testing mode switch...");
        int switch_ok = 1;
        if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) switch_ok = 0;
        else {
            sleep(1);
            if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) switch_ok = 0;
            else {
                usleep(100000);
                sleep(1);
                if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) switch_ok = 0;
                else sleep(1);
            }
        }
        player_core_stop(pc);
        if (switch_ok) printf(" [PASSED]\n");
        else {
            printf(" [FAILED]\n");
            failed_tests++;
        }
        
        // 销毁播放器核心
        player_core_destroy(pc);
        printf("--- Loop %d completed --- (%d/%d passed)\n", loop + 1, total_tests - failed_tests, total_tests);
    }
    
    // 测试4: 资源释放
    total_tests++;
    printf("\n--- Test 4: Resource Cleanup ---\n");
    printf("[TEST] Testing resource cleanup...");
    int cleanup_ok = 1;
    for (int i = 0; i < 5; i++) {
        PlayerCore *pc = player_core_create();
        if (!pc) {
            cleanup_ok = 0;
            break;
        }
        // 测试视频播放后的资源释放
        if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) == 0) {
            usleep(500000);
            player_core_stop(pc);
        }
        player_core_destroy(pc);
    }
    if (cleanup_ok) printf(" [PASSED]\n");
    else {
        printf(" [FAILED]\n");
        failed_tests++;
    }
    
    // 输出测试结果
    printf("\n=========================================\n");
    printf("Test Results:\n");
    printf("Total: %d\n", total_tests);
    printf("Passed: %d\n", total_tests - failed_tests);
    printf("Failed: %d\n", failed_tests);
    printf("=========================================\n");
    
    return failed_tests > 0 ? -1 : 0;
}

int main(int argc, char *argv[])
{
    // 解析命令行参数
    int opt;
    int run_test = 0;
    
    while ((opt = getopt(argc, argv, "t")) != -1) {
        switch (opt) {
            case 't':
                run_test = 1;
                break;
            default:
                printf("Usage: %s [-t]\n", argv[0]);
                printf("  -t    Run stress test\n");
                return 0;
        }
    }
    
    // 如果指定了 -t 参数，运行压力测试
    if (run_test) {
        return run_stress_test();
    }
    
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
    
    lv_freetype_init(8, 16, 256 * 1024);
    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_music();

    setup_ui(&guider_ui);               // 调用GUI Guider生成的UI设置函数，初始化界面
    events_init(&guider_ui);            // 初始化事件处理函数，绑定UI元素与事件
    custom_init(&guider_ui);            // 初始化自定义功能

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