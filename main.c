#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "gui_guider.h"
#include "events_init.h"                // 包含事件初始化相关函数声明
#include "custom.h"                     // 包含自定义功能相关函数声明

#include "ipc/ipc_camera.h"
#include "ipc/ipc_media.h"
// TODO: 驾校辅助系统 UI 完成后，通过 app 开关来启用
// #include "ipc/ipc_uwb.h"
// #include "ipc/ipc_mpu6050.h"
#include "ipc/ipc_audio.h"

#include "ipc/lvgl_dbus_protocol.h"

#define CUSTOM_MEM_TRACE_ENABLE 1
#include "ui/custom/custom_mem_trace.h"

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

static void launch_obd2_service(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        execl("./obd2_service", "obd2_service", NULL);
        perror("execl obd2_service");
        _exit(1);
    } else if (pid > 0) {
        printf("[main] obd2_service launched, pid=%d\n", pid);
    } else {
        perror("fork");
    }
}

static void launch_uwb_service(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        execl("./uwb_service", "uwb_service", NULL);
        perror("execl uwb_service");
        _exit(1);
    } else if (pid > 0) {
        printf("[main] uwb_service launched, pid=%d\n", pid);
    } else {
        perror("fork");
    }
}

static void launch_mpu6050_service(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        execl("./mpu6050_service", "mpu6050_service", NULL);
        perror("execl mpu6050_service");
        _exit(1);
    } else if (pid > 0) {
        printf("[main] mpu6050_service launched, pid=%d\n", pid);
    } else {
        perror("fork");
    }
}

static void launch_audio_service(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        execl("./audio_service", "audio_service", NULL);
        perror("execl audio_service");
        _exit(1);
    } else if (pid > 0) {
        printf("[main] audio_service launched, pid=%d\n", pid);
    } else {
        perror("fork");
    }
}

static void reap_child(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    lvgl_init();

    mem_trace_init();

    signal(SIGCHLD, reap_child);

    launch_audio_service();

    /* TODO: 驾校辅助系统 UI 完成后通过 app 开关来启用 */
    // launch_obd2_service();
    // launch_uwb_service();
    // launch_mpu6050_service();

    // if (ipc_uwb_init(PROTO_BUS_ADDRESS, NULL, NULL) != 0) {
    //     fprintf(stderr, "[main] IPC UWB init failed, running without UWB\n");
    // }

    if (ipc_audio_init(PROTO_BUS_ADDRESS) != 0) {
        fprintf(stderr, "[main] IPC Audio init failed, running without audio control\n");
    }

    // if (ipc_mpu6050_init(PROTO_BUS_ADDRESS, NULL, NULL) != 0) {
    //     fprintf(stderr, "[main] IPC MPU6050 init failed, running without MPU6050\n");
    // }

    /* Initialize extra libraries (PNG, JPEG, etc.) */
    lv_extra_init();
    printf("LVGL extra libraries initialized\n");

    setup_ui(&guider_ui);               // 调用GUI Guider生成的UI设置函数，初始化界面
    events_init(&guider_ui);            // 初始化事件处理函数，绑定UI元素与事件
    custom_init(&guider_ui);            // 初始化自定义功能

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        ipc_camera_dispatch(0);
        ipc_media_dispatch(0);
        // ipc_uwb_dispatch(0);
        // ipc_mpu6050_dispatch(0);
        ipc_audio_dispatch(0);
        usleep(5000);
    }

    ipc_camera_deinit();
    // ipc_uwb_deinit();
    // ipc_mpu6050_deinit();
    ipc_audio_deinit();
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