#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "../player_core.h"

#define TEST_AUDIO_FILE "/home/debian/music/GET IN THE RING - Thank you for dears.flac"
#define TEST_VIDEO_FILE "/home/debian/video/犬夜叉片头.avi"
#define TEST_DURATION 3 // 每次播放的测试时长（秒）
#define TEST_LOOPS 5    // 测试循环次数

static int g_test_running = 1;

// 模拟 custom_media 中的播放流程
// 参考 custom_media_play_video_by_index 和 custom_media_play_song_by_index
int test_custom_media_like_play(PlayerCore *pc) {
    printf("[TEST] Starting custom_media-like play test...\n");
    
    // 模拟视频播放流程
    printf("[TEST] Step 1: Play video\n");
    if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
        printf("[ERROR] Failed to play video\n");
        return -1;
    }
    usleep(100000); // 等待视频准备
    sleep(TEST_DURATION);
    
    // 模拟切换到音频（参考 custom_media_play_song_by_index 的逻辑）
    printf("[TEST] Step 2: Switch to audio\n");
    
    // custom_media 中切换时会先暂停并停止进度更新
    // 这里直接调用 play_audio，内部会停止之前的播放
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf("[ERROR] Failed to switch to audio\n");
        return -1;
    }
    sleep(TEST_DURATION);
    
    // 模拟停止并清理（参考 custom_media_deinit 的逻辑）
    printf("[TEST] Step 3: Stop and cleanup\n");
    
    // 停止播放器核心
    player_core_stop(pc);
    
    // 等待资源释放（参考 custom_media_deinit 中的等待）
    usleep(30000);  // 30ms - 等待解码器线程退出
    usleep(100000); // 100ms - 确保所有操作完成
    
    printf("[TEST] Custom_media-like play test completed\n");
    return 0;
}

// 模拟用户快速操作测试（参考实际使用场景）
int test_quick_switch(PlayerCore *pc) {
    printf("[TEST] Starting quick switch test...\n");
    
    // 快速切换多次 - 模拟用户快速按键
    for (int i = 0; i < 3; i++) {
        printf("[TEST] Quick switch iteration %d\n", i + 1);
        
        // 快速切换到音频
        if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
            printf("[ERROR] Quick switch to audio failed\n");
            return -1;
        }
        usleep(500000); // 500ms后立即切换
        
        // 快速切换到视频
        if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
            printf("[ERROR] Quick switch to video failed\n");
            return -1;
        }
        usleep(500000); // 500ms后立即切换
    }
    
    player_core_stop(pc);
    printf("[TEST] Quick switch test completed\n");
    return 0;
}

// 模拟暂停/继续操作（参考 custom_media_pause 和 custom_media_resume）
int test_pause_resume(PlayerCore *pc) {
    printf("[TEST] Starting pause/resume test...\n");
    
    // 开始播放音频
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf("[ERROR] Failed to play audio\n");
        return -1;
    }
    
    sleep(1);
    
    // 多次暂停/继续
    for (int i = 0; i < 3; i++) {
        printf("[TEST] Pause/resume iteration %d\n", i + 1);
        
        player_core_pause(pc);
        usleep(500000); // 暂停500ms
        
        player_core_resume(pc);
        sleep(1);
    }
    
    player_core_stop(pc);
    printf("[TEST] Pause/resume test completed\n");
    return 0;
}

// 模拟视频播放后退出再重新进入的场景
// 这是用户报告的崩溃场景：播放视频 -> 切换音乐 -> 退出
int test_video_to_music_exit(PlayerCore *pc) {
    printf("[TEST] Starting video->music->exit test (crash scenario)...\n");
    
    // 场景：播放视频
    printf("[TEST] Playing video...\n");
    if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
        printf("[ERROR] Failed to play video\n");
        return -1;
    }
    usleep(100000);
    sleep(2);
    
    // 场景：切换到音乐模式
    printf("[TEST] Switching to music mode...\n");
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf("[ERROR] Failed to switch to music\n");
        return -1;
    }
    sleep(2);
    
    // 场景：停止播放（模拟退出）
    printf("[TEST] Stopping and exiting...\n");
    player_core_stop(pc);
    
    // 等待资源释放
    usleep(30000);
    usleep(100000);
    
    printf("[TEST] Video->music->exit test completed\n");
    return 0;
}

// 测试音频播放
int test_audio_play(PlayerCore *pc) {
    printf("[TEST] Starting audio play test...\n");
    
    int result = player_core_play_audio(pc, TEST_AUDIO_FILE);
    if (result != 0) {
        printf("[ERROR] Failed to play audio\n");
        return -1;
    }
    
    sleep(TEST_DURATION);
    
    player_core_stop(pc);
    printf("[TEST] Audio play test completed\n");
    return 0;
}

// 测试视频播放
int test_video_play(PlayerCore *pc) {
    printf("[TEST] Starting video play test...\n");
    
    int result = player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450);
    if (result != 0) {
        printf("[ERROR] Failed to play video\n");
        return -1;
    }
    
    // 等待视频准备好
    usleep(100000);
    
    sleep(TEST_DURATION);
    
    player_core_stop(pc);
    printf("[TEST] Video play test completed\n");
    return 0;
}

// 测试模式切换
int test_mode_switch(PlayerCore *pc) {
    printf("[TEST] Starting mode switch test...\n");
    
    // 先播放音频
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf("[ERROR] Failed to play audio\n");
        return -1;
    }
    sleep(2);
    
    // 切换到视频
    if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) != 0) {
        printf("[ERROR] Failed to switch to video\n");
        return -1;
    }
    usleep(100000);
    sleep(2);
    
    // 切换回音频
    if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
        printf("[ERROR] Failed to switch back to audio\n");
        return -1;
    }
    sleep(2);
    
    player_core_stop(pc);
    printf("[TEST] Mode switch test completed\n");
    return 0;
}

// 测试资源释放
int test_resource_cleanup() {
    printf("[TEST] Starting resource cleanup test...\n");
    
    for (int i = 0; i < 5; i++) {
        PlayerCore *pc = player_core_create();
        if (!pc) {
            printf("[ERROR] Failed to create player core\n");
            return -1;
        }
        
        // 测试视频播放后的资源释放
        if (player_core_play_video(pc, TEST_VIDEO_FILE, 800, 450) == 0) {
            usleep(500000);
            player_core_stop(pc);
        }
        
        player_core_destroy(pc);
        printf("[TEST] Cleanup iteration %d completed\n", i + 1);
    }
    
    printf("[TEST] Resource cleanup test completed\n");
    return 0;
}

// 测试连续播放
int test_continuous_play(PlayerCore *pc) {
    printf("[TEST] Starting continuous play test...\n");
    
    // 连续播放同一个文件多次
    for (int i = 0; i < 5; i++) {
        printf("[TEST] Continuous play iteration %d\n", i + 1);
        
        if (player_core_play_audio(pc, TEST_AUDIO_FILE) != 0) {
            printf("[ERROR] Failed to play audio\n");
            return -1;
        }
        
        sleep(1);
        player_core_stop(pc);
        usleep(200000); // 200ms间隔
    }
    
    printf("[TEST] Continuous play test completed\n");
    return 0;
}

// 测试并发操作
void *concurrent_test_thread(void *arg) {
    PlayerCore *pc = (PlayerCore *)arg;
    int thread_id = *(int *)arg;
    
    printf("[TEST] Thread %d started\n", thread_id);
    
    while (g_test_running) {
        if (player_core_play_audio(pc, TEST_AUDIO_FILE) == 0) {
            sleep(1);
            player_core_stop(pc);
        }
        usleep(500000);
    }
    
    printf("[TEST] Thread %d exited\n", thread_id);
    return NULL;
}

int test_concurrent_operations() {
    printf("[TEST] Starting concurrent operations test...\n");
    
    PlayerCore *pc = player_core_create();
    if (!pc) {
        printf("[ERROR] Failed to create player core\n");
        return -1;
    }
    
    g_test_running = 1;
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, concurrent_test_thread, &thread_ids[i]);
    }
    
    sleep(5);
    g_test_running = 0;
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    player_core_destroy(pc);
    printf("[TEST] Concurrent operations test completed\n");
    return 0;
}

int main(int argc, char *argv[]) {
    printf("=========================================\n");
    printf("Multimedia Module Stress Test\n");
    printf("=========================================\n");
    printf("Test scenarios simulating real custom_media interaction\n");
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
        
        // 测试1: custom_media风格播放流程
        total_tests++;
        printf("\n--- Test 1: Custom Media Flow ---");
        if (test_custom_media_like_play(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 测试2: 视频->音乐->退出（崩溃场景）
        total_tests++;
        printf("--- Test 2: Video->Music->Exit (Crash Scenario) ---");
        if (test_video_to_music_exit(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 测试3: 模式切换
        total_tests++;
        printf("--- Test 3: Mode Switch ---");
        if (test_mode_switch(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 测试4: 快速切换（模拟用户快速操作）
        total_tests++;
        printf("--- Test 4: Quick Switch ---");
        if (test_quick_switch(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 测试5: 暂停/继续
        total_tests++;
        printf("--- Test 5: Pause/Resume ---");
        if (test_pause_resume(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 测试6: 连续播放
        total_tests++;
        printf("--- Test 6: Continuous Play ---");
        if (test_continuous_play(pc) != 0) {
            failed_tests++;
            printf(" [FAILED]\n");
        } else {
            printf(" [PASSED]\n");
        }
        
        // 销毁播放器核心
        player_core_destroy(pc);
        
        printf("--- Loop %d completed ---", loop + 1);
        printf(" (%d/%d passed)\n", total_tests - failed_tests, total_tests);
    }
    
    // 测试7: 资源释放（单独测试）
    total_tests++;
    printf("\n--- Test 7: Resource Cleanup ---");
    if (test_resource_cleanup() != 0) {
        failed_tests++;
        printf(" [FAILED]\n");
    } else {
        printf(" [PASSED]\n");
    }
    
    // 输出测试结果
    printf("\n=========================================\n");
    printf("Test Results:\n");
    printf("Total: %d\n", total_tests);
    printf("Passed: %d\n", total_tests - failed_tests);
    printf("Failed: %d\n", failed_tests);
    printf("=========================================\n");
    
    if (failed_tests > 0) {
        return -1;
    }
    
    return 0;
}