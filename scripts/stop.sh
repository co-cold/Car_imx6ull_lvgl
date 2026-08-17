#!/bin/sh
# stop.sh — 关闭所有进程

echo "========== 关闭中 =========="

for proc in lvgl_demo obd2_service camera_service media_service dbus-daemon; do
    kill -9 $(pidof $proc) 2>/dev/null
done

rm -f /tmp/lvgl_demo.pid /tmp/lvgl_dbus.pid /tmp/lvgl-dbus-session
echo "已全部关闭"