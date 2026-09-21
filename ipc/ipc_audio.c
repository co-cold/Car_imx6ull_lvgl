/*
 * ipc_audio.c — 音频 IPC 实现
 *
 * 通过 D-Bus 远程方法调用 audio_service，
 * 实现 set_volume / get_volume 指令。
 */
#include "ipc_audio.h"
#include "lvgl_dbus_protocol.h"
#include "ipc_base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

static DBusConnection *g_conn = NULL;
static int g_service_connected = 0;

static DBusHandlerResult filter_cb(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)conn;
    (void)data;

    if (dbus_message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged")) {
        const char *nm = NULL, *old_owner = NULL, *new_owner = NULL;
        if (dbus_message_get_args(msg, NULL,
                DBUS_TYPE_STRING, &nm,
                DBUS_TYPE_STRING, &old_owner,
                DBUS_TYPE_STRING, &new_owner,
                DBUS_TYPE_INVALID)) {
            if (nm && strcmp(nm, AUDIO_SERVICE_NAME) == 0) {
                g_service_connected = (new_owner && new_owner[0] != '\0') ? 1 : 0;
                printf("[ipc_audio] service %s\n",
                       g_service_connected ? "appeared" : "disappeared");
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

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

    /* 监听 NameOwnerChanged 实现服务发现 */
    ipc_base_watch_service(g_conn, AUDIO_SERVICE_NAME);

    if (!dbus_connection_add_filter(g_conn, filter_cb, NULL, NULL)) {
        fprintf(stderr, "[ipc_audio] add_filter failed\n");
        dbus_connection_unref(g_conn);
        g_conn = NULL;
        return -1;
    }

    /* 等待 audio_service 上线 */
    if (ipc_base_wait_for_service(g_conn, AUDIO_SERVICE_NAME, 25) == 0)
        g_service_connected = 1;
    else
        fprintf(stderr, "[ipc_audio] WARNING: audio_service not found after 500ms\n");

    return 0;
}

void ipc_audio_deinit(void)
{
    if (g_conn) {
        dbus_connection_close(g_conn);
    }
    ipc_base_disconnect(&g_conn, filter_cb);
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
        AUDIO_SERVICE_NAME, AUDIO_OBJECT_PATH, AUDIO_SERVICE_NAME, method);
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
        AUDIO_SERVICE_NAME, AUDIO_OBJECT_PATH, AUDIO_SERVICE_NAME, method);
    if (!msg) return -1;

    dbus_message_append_args(msg, DBUS_TYPE_DOUBLE, &value, DBUS_TYPE_INVALID);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(g_conn, msg, 5000, NULL);
    dbus_message_unref(msg);

    if (!reply) return -1;

    dbus_int32_t ret = -1;
    dbus_message_get_args(reply, NULL, DBUS_TYPE_INT32, &ret, DBUS_TYPE_INVALID);
    dbus_message_unref(reply);
    return (ret == 0) ? 0 : -1;
}

static int call_double(const char *method, double *out_value)
{
    if (!g_conn) return -1;

    DBusMessage *msg = dbus_message_new_method_call(
        AUDIO_SERVICE_NAME, AUDIO_OBJECT_PATH, AUDIO_SERVICE_NAME, method);
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
    int result = send_double(AUDIO_METHOD_SET_VOLUME, (double)volume);
    if (result != 0) {
        fprintf(stderr, "[ipc_audio] Failed to set volume: %.2f\n", volume);
    }
    return result;
}

float ipc_audio_get_volume(void)
{
    double v = 0.5;
    if (call_double(AUDIO_METHOD_GET_VOLUME, &v) != 0) {
        fprintf(stderr, "[ipc_audio] Failed to get volume\n");
        v = 0.5; // 默认值
    }
    return (float)v;
}