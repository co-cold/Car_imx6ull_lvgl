#!/bin/sh
# debug.sh — 调试模式启动（gdbserver 接管 lvgl_demo）

DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"

# 先清理残留
./stop.sh 2>/dev/null
sleep 1

echo "========== 启动 D-Bus 守护进程 =========="
dbus-daemon --session --address=unix:path=/tmp/lvgl-dbus-session &
echo $! > /tmp/lvgl_dbus.pid
sleep 1

echo "========== 启动 LVGL 主程序 (gdbserver) =========="
echo "请在 VS Code 中按 F5 连接，地址 192.168.7.2:2001"
nohup gdbserver :2001 ./lvgl_demo > /tmp/gdbserver.log 2>&1 &
echo "gdbserver 已在后台运行，日志: /tmp/gdbserver.log"
echo ""
echo "停止: sudo ./stop.sh"