#!/bin/sh
# start.sh — 启动所有服务（后台运行）

DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"

export LD_LIBRARY_PATH="$DIR:$LD_LIBRARY_PATH"

# 先清理残留
./stop.sh 2>/dev/null
sleep 1

echo "========== 启动 D-Bus 守护进程 =========="
dbus-daemon --session --address=unix:path=/tmp/lvgl-dbus-session &
echo $! > /tmp/lvgl_dbus.pid
sleep 1

echo "========== 启动 LVGL 主程序 =========="
nohup ./lvgl_demo > /dev/null 2>&1 &
echo $! > /tmp/lvgl_demo.pid
echo "已启动，PID=$(cat /tmp/lvgl_demo.pid)"

echo ""
echo "使用 ./stop.sh 关闭所有进程"