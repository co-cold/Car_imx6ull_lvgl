# Multimedia 模块架构设计文档

## 1. 概述

Multimedia 模块是一个基于 FFmpeg 和 ALSA 的多媒体播放器核心，专为嵌入式设备（如 i.MX6ULL）优化设计，支持音频播放（FLAC/MP3等）和视频播放（MP4等）。

**版本历史**：
- v1.0 (2026-05-10)：初始版本
- v1.1 (2026-05-12)：修复资源泄漏、UI状态同步、崩溃问题

## 2. 整体架构

### 2.1 架构设计思想

**分层架构**：采用经典的三层架构模式，实现关注点分离和模块解耦：

1. **UI层**：负责用户交互和界面展示，与业务逻辑完全解耦
2. **核心层**：作为中间协调层，管理播放状态和资源生命周期
3. **驱动层**：直接与底层系统交互（FFmpeg、ALSA）

**模块化设计**：每个模块职责单一，通过清晰的接口通信，便于独立测试和维护。

**线程模型**：采用生产者-消费者模式，解码线程生产数据，输出线程消费数据，通过环形缓冲区解耦。

### 2.2 架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        UI 层 (LVGL)                            │
│  - 音乐播放界面 (screen_music)                                 │
│  - 视频播放界面 (screen_video)                                 │
│  - 状态同步 (custom_media)                                    │
└───────────────────────────┬───────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                     播放器核心层 (PlayerCore)                  │
│  - 播放状态管理（状态机模式）                                    │
│  - 音视频同步控制（时间戳对齐）                                  │
│  - 资源生命周期管理（RAII思想）                                  │
│  - 模式切换协调（策略模式）                                      │
│  - 播放列表管理                                                 │
└───────────────────────────┬───────────────────────────────────┘
                            │
       ┌──────────────┬─────┼─────┬──────────────┐
       ▼              ▼     ▼     ▼              ▼
┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐
│   Audio   │ │   Video   │ │   Media   │ │   Utils   │
│   Layer   │ │   Layer   │ │   Layer   │ │   Layer   │
│-decoder   │ │-decoder   │ │-playlist  │ │-ring_buf  │
│-output    │ │-doublebuf │ │-metadata  │ │           │
│-mixer     │ │-sws_scale │ │           │ │           │
└───────────┘ └───────────┘ └───────────┘ └───────────┘
```

### 2.3 设计模式应用

| 设计模式 | 应用场景 | 作用 |
|----------|----------|------|
| **状态机模式** | PlayerCore状态管理 | 清晰定义状态转换规则，避免状态混乱 |
| **策略模式** | 音视频播放切换 | 统一接口，不同实现，易于扩展 |
| **生产者-消费者** | 音频数据传输 | 解耦解码与输出，提高并发性能 |
| **双缓冲模式** | 视频帧显示 | 避免解码线程与UI线程竞争 |
| **单例模式** | MediaContext | 全局共享状态，简化UI集成 |

## 3. 核心组件说明

### 3.1 PlayerCore（播放器核心）

**职责**：统一管理音视频播放状态、协调解码器和输出模块、处理播放控制指令。

**设计思想**：作为整个多媒体模块的核心控制器，采用**门面模式**封装底层复杂逻辑，向上层提供简洁统一的接口。

**状态机设计**：
```
            play()
    ┌──────────────────┐
    ▼                  │
STOPPED ───────────→ PLAYING
    │                  │
    │ stop()           │ pause()
    │                  ▼
    │←────────────── PAUSED
           resume() / stop()
```

**状态定义**（实际实现）：
- `0`：idle（空闲状态）
- `1`：playing（播放中）
- `2`：paused（暂停状态）

**关键数据结构**（实际实现）：
```c
typedef struct PlayerCore {
    RingBuffer *rb;           // 音频环形缓冲区
    AudioOutput *ao;          // 音频输出
    AudioDecoder *ad;         // 音频解码器
    VideoDecoder *vd;         // 视频解码器
    VideoDoubleBuf *vdb;      // 视频双缓冲
    int state;                // 播放状态：0-idle, 1-playing, 2-paused
    pthread_mutex_t lock;     // 状态保护互斥锁
    int video_mode;           // 0-audio only, 1-video
    int sample_rate;          // 当前采样率
    float volume;             // 当前音量值（0.0-1.0）
    int video_width;          // 视频目标宽度
    int video_height;         // 视频目标高度
    Playlist *audio_playlist; // 音频播放列表
    Playlist *video_playlist; // 视频播放列表
} PlayerCore;
```

**核心接口**：
| 接口 | 功能 | 设计考虑 |
|------|------|----------|
| `player_core_create()` | 创建播放器核心实例 | 延迟初始化，按需创建 |
| `player_core_destroy()` | 销毁播放器核心实例 | 递归释放所有子资源 |
| `player_core_play_audio()` | 播放音频文件 | 自动停止当前播放，状态切换 |
| `player_core_play_video()` | 播放视频文件 | 初始化视频解码器和双缓冲 |
| `player_core_stop()` | 停止播放 | 按正确顺序释放资源 |
| `player_core_pause()` | 暂停播放 | 暂停音频输出和视频解码 |
| `player_core_resume()` | 恢复播放 | 从暂停状态恢复播放 |
| `player_core_seek()` | 跳转到指定时间位置 | 同步音频和视频解码器 |
| `player_core_get_position()` | 获取当前播放位置 | 返回秒数 |
| `player_core_get_duration()` | 获取媒体总时长 | 返回秒数 |
| `player_core_get_state()` | 获取当前播放状态 | 返回0/1/2 |
| `player_core_set_volume()` | 设置音量 | 保存音量值用于切换恢复 |
| `player_core_get_volume()` | 获取当前音量 | 返回音量值 |
| `player_core_get_video_frame()` | 获取就绪的视频帧 | RGB565格式 |
| `player_core_release_video_frame()` | 释放视频帧 | 允许解码器写入新帧 |

### 3.2 Audio Layer（音频层）

**AudioDecoder**：基于 FFmpeg 的音频解码器，负责解析音频流并解码为 PCM 数据。

**AudioOutput**：基于 ALSA 的音频输出模块，负责将 PCM 数据写入音频硬件。

**关键配置**：
- period: 2048 帧
- buffer_size: 16384 帧 (8 * period)
- 支持格式: S16_LE

### 3.3 Video Layer（视频层）

**VideoDecoder**：基于 FFmpeg 的视频解码器，负责解析视频流、解码帧并缩放至目标分辨率。

**VideoDoubleBuf**：双缓冲机制，避免解码线程与 UI 线程的数据竞争。

**性能优化配置**（针对 i.MX6ULL）：
- 目标分辨率: 800x450（在 `custom_media.c` 中通过 `VIDEO_WIDTH/VIDEO_HEIGHT` 宏定义）
- 帧更新频率: 15fps (67ms 间隔)
- 帧同步: 每4帧等待一次

### 3.4 Utils Layer（工具层）

**RingBuffer**：线程安全的环形缓冲区，用于音频数据传输。采用**生产者-消费者模式**，通过互斥锁和条件变量实现线程安全。

**设计要点**：
- 使用 `pthread_mutex_t` 保护缓冲区访问
- 使用 `pthread_cond_t` 实现缓冲区满/空等待
- 添加 `rb_wakeup_all()` 支持强制唤醒，避免线程阻塞

### 3.5 CustomMedia（UI集成层）

**职责**：作为 LVGL UI 与 PlayerCore 之间的适配层，处理 UI 事件和状态同步。

**设计考虑**：
- 采用**适配器模式**，将 LVGL 事件转换为播放器操作
- 管理 UI 控件状态与播放器状态的同步
- 处理定时器回调中的视频帧更新

## 4. 数据流设计

### 4.1 音频播放流程

```
音频文件 → AudioDecoder → PCM数据 → RingBuffer → AudioOutput → ALSA → 扬声器
```

**线程模型**：
- 主线程：UI 控制、状态管理
- 解码线程 (`audio_decoder_thread`)：音频解码并写入环形缓冲区
- 输出线程 (`audio_output_thread`)：从环形缓冲区读取数据并输出到 ALSA
- **暂停处理**：暂停时停止解码线程，但保持输出线程运行（消耗缓冲区剩余数据）

### 4.2 视频播放流程

```
视频文件 → VideoDecoder → YUV帧 → sws_scale → RGB帧 → VideoDoubleBuf
                                                              │
                                                              ▼
                                                       LVGL定时器 → 显示
```

**线程模型**：
- 主线程：UI 控制
- 视频解码线程 (`video_decoder_thread`)：解码帧并提交到双缓冲
- LVGL定时器 (`custom_media_update_video_frame`)：约67ms触发一次，更新视频帧显示
- **暂停处理**：暂停时停止解码线程，保持最后一帧显示

## 5. 接口设计

### 5.1 对外接口（custom_media）

**设计原则**：接口设计遵循**最小惊讶原则**，参数命名清晰，错误返回值明确。

| 函数 | 参数 | 返回值 | 功能 |
|------|------|--------|------|
| `custom_media_init()` | `ui` - UI实例指针 | `void` | 初始化媒体模块，绑定UI控件 |
| `custom_media_deinit()` | 无 | `void` | 反初始化媒体模块，释放所有资源 |
| `custom_media_play_song_by_index()` | `idx` - 歌曲索引 | `int` | 按索引播放歌曲，更新UI状态 |
| `custom_media_play_next()` | 无 | `void` | 播放下一首，自动循环 |
| `custom_media_play_prev()` | 无 | `void` | 播放上一首，自动循环 |
| `custom_media_play_video()` | `path` - 视频路径, `w`/`h` - 分辨率 | `int` | 播放视频，初始化视频显示 |
| `custom_media_stop()` | 无 | `void` | 停止播放，重置UI状态 |
| `custom_media_pause()` | 无 | `void` | 暂停播放，保持当前状态 |
| `custom_media_set_volume()` | `vol` - 音量(0-100) | `void` | 设置音量，同步到ALSA混音器 |
| `custom_media_scan_music_dir()` | `dir` - 目录路径 | `int` | 扫描音乐目录，构建播放列表 |

### 5.2 内部接口

#### PlayerCore
```c
PlayerCore *player_core_create(void);
void player_core_destroy(PlayerCore *pc);
int player_core_play_audio(PlayerCore *pc, const char *file_path);
int player_core_play_video(PlayerCore *pc, const char *file_path, int dst_width, int dst_height);
void player_core_stop(PlayerCore *pc);
void player_core_pause(PlayerCore *pc);
void player_core_resume(PlayerCore *pc);
void player_core_seek(PlayerCore *pc, double seconds);
double player_core_get_position(PlayerCore *pc);
double player_core_get_duration(PlayerCore *pc);
int player_core_get_state(PlayerCore *pc);
void player_core_set_volume(PlayerCore *pc, float volume);
float player_core_get_volume(PlayerCore *pc);
uint8_t* player_core_get_video_frame(PlayerCore *pc);
void player_core_release_video_frame(PlayerCore *pc);
// 播放列表管理
void player_core_set_audio_playlist(PlayerCore *pc, Playlist *playlist);
void player_core_set_video_playlist(PlayerCore *pc, Playlist *playlist);
int player_core_play_audio_by_index(PlayerCore *pc, int idx);
int player_core_play_video_by_index(PlayerCore *pc, int idx, int width, int height);
int player_core_play_next(PlayerCore *pc);
int player_core_play_prev(PlayerCore *pc);
```

#### Audio
```c
AudioDecoder *audio_decoder_init(const char *file_path, RingBuffer *rb);
void audio_decoder_start(AudioDecoder *ad);
void audio_decoder_stop(AudioDecoder *ad);
void audio_decoder_free(AudioDecoder *ad);

AudioOutput *audio_output_init(unsigned int sample_rate, int channels, int period_size, RingBuffer *rb);
void audio_output_start(AudioOutput *ao);
void audio_output_pause(AudioOutput *ao);
void audio_output_resume(AudioOutput *ao);
void audio_output_stop(AudioOutput *ao);
void audio_output_free(AudioOutput *ao);
void audio_output_set_volume(AudioOutput *ao, float volume);
void audio_output_set_volume_all(AudioOutput *ao, float volume);
```

#### Video
```c
VideoDecoder *video_decoder_init(const char *file_path, VideoDoubleBuf *vdb, int dst_w, int dst_h);
void video_decoder_start(VideoDecoder *vd);
void video_decoder_stop(VideoDecoder *vd);
void video_decoder_pause(VideoDecoder *vd);
void video_decoder_resume(VideoDecoder *vd);
void video_decoder_free(VideoDecoder *vd);

VideoDoubleBuf *vdb_create(int size);
void vdb_destroy(VideoDoubleBuf *vdb);
// 解码线程使用
uint8_t *vdb_get_write_buf(VideoDoubleBuf *vdb);
void vdb_commit(VideoDoubleBuf *vdb);
// UI线程使用
uint8_t *vdb_get_ready_buf(VideoDoubleBuf *vdb);
void vdb_release(VideoDoubleBuf *vdb);
```

## 6. 错误处理与稳定性

### 6.1 常见错误及处理

| 错误类型 | 原因 | 处理策略 |
|----------|------|----------|
| XRUN错误 | 音频缓冲区下溢/上溢 | 使用`snd_pcm_recover()`恢复，或调用`snd_pcm_prepare()`重新初始化 |
| 混音器控件缺失 | 设备无Master/PCM控件 | 使用Headphone/Speaker/Playback控件 |
| 段错误 | 资源未释放导致重复初始化 | 在`player_core_stop`中确保释放所有资源 |
| 音画不同步 | i.MX6ULL性能不足 | 降低分辨率、帧率，添加帧同步控制 |
| 音频设备忙 | 设备未正确释放 | 在`audio_output_free`中添加`snd_pcm_drop`和`snd_pcm_drain` |

### 6.2 资源管理规则

1. **初始化顺序**：RingBuffer → Decoder → Output
2. **释放顺序**：Output → Decoder → RingBuffer
3. **状态检查**：所有操作前检查资源有效性
4. **异常清理**：任何步骤失败时清理已分配资源
5. **线程安全**：使用互斥锁保护共享资源访问

### 6.3 已修复问题记录

| 问题描述 | 修复位置 | 修复策略 |
|----------|----------|----------|
| 音频设备资源泄漏 | `audio_output.c` | 添加`snd_pcm_drop()`丢弃缓冲区、`snd_pcm_drain()`等待排空，最后`snd_pcm_close()`关闭设备 |
| 视频帧定时器回调崩溃 | `custom_media.c` | 添加多层有效性检查，防止访问已释放资源 |
| 视频列表播放图标状态错误 | `custom_media.c` | 在`video_list_item_click_event`中更新播放按钮图标 |
| 视频关闭按钮退出界面 | `events_init.c` | 修改为停止播放但不退出界面 |
| 环形缓冲区线程阻塞 | `ring_buffer.c` | 添加`rb_wakeup_all()`函数唤醒所有等待线程 |

## 7. 性能优化策略（i.MX6ULL）

### 7.1 视频优化
- **分辨率配置**：固定为800x450（与UI控件尺寸匹配）
- **帧率限制**：从24fps降至15fps
- **帧同步控制**：每4帧等待一次，避免视频过快
- **色彩空间转换**：使用FFmpeg的`sws_scale()`进行软件缩放；条件允许时可考虑`libyuv`或硬件加速
- **解码线程数**：设置`AVCodecContext.thread_count`为CPU核心数，启用多线程解码

### 7.2 音频优化
- **缓冲区配置**：period=2048, buffer=16384，平衡延迟与稳定性
- **格式选择**：使用S16_LE，减少CPU处理开销
- **线程优先级**：音频输出线程设为较高优先级

### 7.3 内存优化
- **双缓冲复用**：VideoDoubleBuf循环使用两个缓冲区
- **按需分配**：仅在播放时创建解码器实例
- **及时释放**：停止播放立即释放资源

## 8. 配置与依赖

### 8.1 LVGL配置
- `LV_USE_FS_POSIX`：启用POSIX文件系统支持
- `LV_COLOR_DEPTH=16`：设置颜色深度为16位（RGB565）
- `LV_USE_TIMER`：启用定时器功能
- `LV_USE_IMG`：启用图像控件
- `LV_USE_PNG`：启用PNG图片解码器
- `LV_USE_JPEG`：启用JPEG图片解码器

### 8.2 外部依赖
- **FFmpeg**：libavformat, libavcodec, libswscale, libavutil, libswresample（可选，用于音频重采样）
- **ALSA**：libasound
- **LVGL**：图形库

## 9. 代码位置

| 模块 | 文件路径 | 职责说明 |
|------|----------|----------|
| PlayerCore | `multimedia/player_core.c` | 播放器核心控制器，状态管理 |
| AudioDecoder | `multimedia/audio/audio_decoder.c` | FFmpeg音频解码，PCM输出 |
| AudioOutput | `multimedia/audio/audio_output.c` | ALSA音频输出，混音器控制 |
| VideoDecoder | `multimedia/video/video_decoder.c` | FFmpeg视频解码，帧缩放 |
| VideoDoubleBuf | `multimedia/video/video_double_buf.c` | 视频双缓冲，线程安全 |
| Playlist | `multimedia/media/playlist.c` | 播放列表管理 |
| MediaMetadata | `multimedia/media/media_metadata.c` | 音频元数据解析 |
| RingBuffer | `multimedia/utils/ring_buffer.c` | 线程安全环形缓冲区 |
| CustomMedia | `ui/custom/custom_media.c` | LVGL UI适配层 |
| Events | `ui/generated/events_init.c` | LVGL事件处理 |
| Test | `multimedia/test/multimedia_test.c` | 压力测试程序 |

## 10. 扩展建议

1. **支持更多格式**：添加对WMV、MKV等格式的支持
2. **硬件加速**：利用i.MX6的VPU进行视频解码
3. **网络流媒体**：添加HTTP/RTSP流媒体支持
4. **播放列表管理**：实现播放列表的增删改查功能
5. **均衡器**：添加音频均衡器功能

## 11. 测试与验证

### 11.1 压力测试场景

| 测试场景 | 测试内容 | 预期结果 |
|----------|----------|----------|
| 视频→音乐→退出 | 播放视频→切换音乐→停止 | 无崩溃，资源正确释放 |
| 快速切换 | 500ms间隔快速切换音视频 | 无崩溃，状态正确切换 |
| 暂停/继续循环 | 连续多次暂停/继续操作 | 状态正确切换 |
| 资源释放 | 连续创建/销毁播放器 | 无内存泄漏 |

### 11.2 测试工具

- **multimedia_test.c**：独立压力测试程序（`-t`参数运行）
- **valgrind**：内存泄漏检测
- **stress_test.txt**：测试结果记录文件

---
*文档版本: v1.1*  
*最后更新: 2026-05-12*