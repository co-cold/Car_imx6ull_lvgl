#ifndef LVGL_DBUS_PROTOCOL_H
#define LVGL_DBUS_PROTOCOL_H

/*
 * lvgl_dbus_protocol.h — 项目全局 D-Bus 协议定义
 *
 * 所有 Service 和 IPC Client 共享同一套 D-Bus 字符串常量，
 * 避免服务端和客户端各自硬编码导致的维护成本。
 *
 * 规范说明（参考 freedesktop.org D-Bus 命名惯例）：
 *   SERVICE_NAME  = 反向域名 + 模块名        （例: com.lvgl.demo.Camera）
 *   OBJECT_PATH   = /反向域名/模块名          （例: /com/lvgl/demo/Camera）
 *   IFACE_NAME    = 反向域名 + 模块名          （例: com.lvgl.demo.Camera）
 *   SIGNAL_*      = 信号名，服务端发送、客户端订阅
 *   METHOD_*      = 方法名，客户端调用、服务端处理
 */

/* ======== 总线地址 ======== */
#define PROTO_BUS_ADDRESS "unix:path=/tmp/lvgl-dbus-session"

/* ================================================================
 * Camera 协议
 * ================================================================ */
#define CAMERA_SERVICE_NAME "com.lvgl.demo.Camera"
#define CAMERA_OBJECT_PATH  "/com/lvgl/demo/Camera"
#define CAMERA_IFACE_NAME   "com.lvgl.demo.Camera"

#define CAMERA_METHOD_START "Start"
#define CAMERA_METHOD_STOP  "Stop"
#define CAMERA_SIGNAL_FRAME "FrameReady"

/* ================================================================
 * Media 协议
 * ================================================================ */
#define MEDIA_SERVICE_NAME "com.lvgl.demo.Media"
#define MEDIA_OBJECT_PATH  "/com/lvgl/demo/Media"
#define MEDIA_IFACE_NAME   "com.lvgl.demo.Media"

#define MEDIA_METHOD_PLAY_AUDIO    "PlayAudio"
#define MEDIA_METHOD_PLAY_VIDEO    "PlayVideo"
#define MEDIA_METHOD_PAUSE         "Pause"
#define MEDIA_METHOD_RESUME        "Resume"
#define MEDIA_METHOD_STOP          "Stop"
#define MEDIA_METHOD_SEEK          "Seek"
#define MEDIA_METHOD_GET_POSITION  "GetPosition"
#define MEDIA_METHOD_GET_DURATION  "GetDuration"
#define MEDIA_METHOD_GET_STATE     "GetState"
#define MEDIA_METHOD_GET_INFO      "GetPlaybackInfo"
#define MEDIA_SIGNAL_FRAME         "FrameReady"
#define MEDIA_SIGNAL_COMPLETE      "PlaybackComplete"

/* ================================================================
 * Audio 协议
 * ================================================================ */
#define AUDIO_SERVICE_NAME "com.lvgl.demo.Audio"
#define AUDIO_OBJECT_PATH  "/com/lvgl/demo/Audio"
#define AUDIO_IFACE_NAME   "com.lvgl.demo.Audio"

#define AUDIO_METHOD_SET_VOLUME "SetVolume"
#define AUDIO_METHOD_GET_VOLUME "GetVolume"

/* ================================================================
 * UWB 协议
 * ================================================================ */
#define UWB_SERVICE_NAME "com.lvgl.demo.UWB"
#define UWB_OBJECT_PATH  "/com/lvgl/demo/UWB"
#define UWB_IFACE_NAME   "com.lvgl.demo.UWB"

#define UWB_SIGNAL_DATA   "UwbDataUpdated"
#define UWB_SIGNAL_STATUS "StatusChanged"

/* ================================================================
 * MPU6050 协议
 * ================================================================ */
#define MPU6050_SERVICE_NAME "com.lvgl.demo.MPU6050"
#define MPU6050_OBJECT_PATH  "/com/lvgl/demo/MPU6050"
#define MPU6050_IFACE_NAME   "com.lvgl.demo.MPU6050"

#define MPU6050_SIGNAL_DATA "Mpu6050DataUpdated"

/* ================================================================
 * OBD2 协议  (D-Bus 名沿用 CAN，保持兼容)
 * ================================================================ */
#define OBD2_SERVICE_NAME "com.lvgl.demo.CAN"
#define OBD2_OBJECT_PATH  "/com/lvgl/demo/CAN"
#define OBD2_IFACE_NAME   "com.lvgl.demo.CAN"

#define OBD2_SIGNAL_ENCODER "EncoderUpdated"

#endif /* LVGL_DBUS_PROTOCOL_H */