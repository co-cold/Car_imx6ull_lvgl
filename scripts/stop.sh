#!/bin/sh
# stop.sh — 关闭所有进程

echo "========== 关闭中 =========="

for proc in lvgl_demo obd2_service camera_service media_service audio_service gdbserver; do
    kill -9 $(pidof $proc) 2>/dev/null
done

# 不要杀系统 dbus-daemon，只清理我们创建的 D-Bus 会话
if [ -f /tmp/lvgl_dbus.pid ]; then
    kill -9 $(cat /tmp/lvgl_dbus.pid) 2>/dev/null
fi

rm -f /tmp/lvgl_demo.pid /tmp/lvgl_dbus.pid /tmp/lvgl-dbus-session
echo "已全部关闭"