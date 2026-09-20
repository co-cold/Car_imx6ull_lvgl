/*
 * ipc_audio.c — 音频 IPC 实现
 *
 * 通过 D-Bus 远程方法调用 audio_service，
 * 实现 set_volume / get_volume 指令。
 */
#include "ipc_audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <string.h>

static DBusConnection *g_conn = NULL;

int ipc_audio_init(const char *bus_address)
{
    DBusError err;
    dbus_error_init(&err);

    // 如果传入的是 "session"，则连接到会话总线（自动注册）
    if (strcmp(bus_address, "session") == 0) {
        g_conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    } else {
        g_conn = dbus_connection_open(bus_address, &err);
        if (g_conn && !dbus_error_is_set(&err)) {
            dbus_bus_register(g_conn, &err);
        }
    }
    
    if (!g_conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_audio] Failed to open connection: %s\n", err.message);
        dbus_error_free(&err);
        if (g_conn) {
            dbus_connection_unref(g_conn);
            g_conn = NULL;
        }
        return -1;
    }

    dbus_connection_set_exit_on_disconnect(g_conn, FALSE);
    return 0;
}

void ipc_audio_deinit(void)
{
    if (g_conn) {
        dbus_connection_close(g_conn);
        dbus_connection_unref(g_conn);
        g_conn = NULL;
    }
}

void ipc_audio_dispatch(int timeout_ms)
{
    if (g_conn) {
        dbus_connection_read_write(g_conn, timeout_ms);
    }
}

static int send_simple(const char *method)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        "com.lvgl.demo.Audio", "/com/lvgl/demo/Audio", "com.lvgl.demo.Audio", method);
    if (!msg) return -1;

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(g_conn, msg, 5000, NULL);
    dbus_message_unref(msg);

    int result = reply ? 0 : -1;
    if (reply) dbus_message_unref(reply);
    return result;
}

static int send_double(const char *method, double value)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        "com.lvgl.demo.Audio", "/com/lvgl/demo/Audio", "com.lvgl.demo.Audio", method);
    if (!msg) return -1;

    dbus_message_append_args(msg, DBUS_TYPE_DOUBLE, &value, DBUS_TYPE_INVALID);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(g_conn, msg, 5000, NULL);
    dbus_message_unref(msg);

    int result = reply ? 0 : -1;
    if (reply) dbus_message_unref(reply);
    return result;
}

static int call_double(const char *method, double *out_value)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        "com.lvgl.demo.Audio", "/com/lvgl/demo/Audio", "com.lvgl.demo.Audio", method);
    if (!msg) return -1;

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(g_conn, msg, 5000, NULL);
    dbus_message_unref(msg);

    if (reply) {
        if (dbus_message_get_args(reply, NULL, DBUS_TYPE_DOUBLE, out_value, DBUS_TYPE_INVALID)) {
            dbus_message_unref(reply);
            return 0;
        }
        dbus_message_unref(reply);
    }
    return -1;
}

int ipc_audio_set_volume(float volume)
{
    int result = send_double("SetVolume", (double)volume);
    if (result != 0) {
        fprintf(stderr, "[ipc_audio] Failed to set volume: %.2f\n", volume);
    }
    return result;
}

float ipc_audio_get_volume(void)
{
    double v = 0.5;
    if (call_double("GetVolume", &v) != 0) {
        fprintf(stderr, "[ipc_audio] Failed to get volume\n");
        v = 0.5; // 默认值
    }
    return (float)v;
}