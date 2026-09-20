/*
 * audio_service.c — 音频 D-Bus 服务
 *
 * 功能：通过 D-Bus 方法调用实现音量控制 set_volume/get_volume，
 *       与 ALSA mixer 硬件交互控制系统音量。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dbus/dbus.h>
#include "alsa_mixer.h"

#define SERVICE_NAME   "com.lvgl.demo.Audio"
#define OBJECT_PATH    "/com/lvgl/demo/Audio"  
#define INTERFACE_NAME "com.lvgl.demo.Audio"
#define BUS_ADDRESS    "unix:path=/tmp/lvgl-dbus-session"

static volatile int g_running = 1;
static DBusConnection *g_conn = NULL;
static alsa_mixer_t *g_am = NULL;

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

static DBusHandlerResult message_filter(DBusConnection *conn, DBusMessage *msg, void *data)
{
    (void)data;
    
    if (dbus_message_is_method_call(msg, INTERFACE_NAME, "SetVolume")) {
        double vol = 0.0;
        if (dbus_message_get_args(msg, NULL, DBUS_TYPE_DOUBLE, &vol, DBUS_TYPE_INVALID)) {
            printf("[audio_service] Received SetVolume: %.2f\n", vol);
            if (g_am) {
                alsa_mixer_set_volume(g_am, (float)vol);
                printf("[audio_service] Volume set successfully\n");
            } else {
                fprintf(stderr, "[audio_service] ALSA mixer not initialized\n");
            }
            DBusMessage *reply = dbus_message_new_method_return(msg);
            if (reply) {
                dbus_connection_send(conn, reply, NULL);
                dbus_connection_flush(conn);
                dbus_message_unref(reply);
            }
        } else {
            fprintf(stderr, "[audio_service] Failed to parse SetVolume args\n");
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    
    if (dbus_message_is_method_call(msg, INTERFACE_NAME, "GetVolume")) {
        float vol = g_am ? alsa_mixer_get_volume(g_am) : 0.5f;
        DBusMessage *reply = dbus_message_new_method_return(msg);
        if (reply) {
            double dvol = (double)vol;
            dbus_message_append_args(reply, DBUS_TYPE_DOUBLE, &dvol, DBUS_TYPE_INVALID);
            dbus_connection_send(conn, reply, NULL);
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    DBusError err;
    dbus_error_init(&err);

    // 连接到指定的 D-Bus 会话地址
    g_conn = dbus_connection_open(BUS_ADDRESS, &err);
    if (!g_conn) {
        fprintf(stderr, "[audio_service] Failed to connect to D-Bus: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }

    dbus_bus_register(g_conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[audio_service] Failed to register with bus: %s\n", err.message);
        dbus_error_free(&err);
        dbus_connection_unref(g_conn);
        return 1;
    }

    if (dbus_bus_request_name(g_conn, SERVICE_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &err) != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "[audio_service] Failed to acquire service name: %s\n", err.message);
        dbus_error_free(&err);
        dbus_connection_unref(g_conn);
        return 1;
    }

    dbus_connection_add_filter(g_conn, message_filter, NULL, NULL);
    dbus_connection_flush(g_conn);

    g_am = alsa_mixer_init();
    if (!g_am) {
        fprintf(stderr, "[audio_service] Failed to initialize ALSA mixer\n");
        dbus_connection_unref(g_conn);
        return 1;
    }

    // Set default volume to 50%
    alsa_mixer_set_volume(g_am, 0.5f);

    printf("[audio_service] Started successfully\n");

    while (g_running) {
        dbus_connection_read_write_dispatch(g_conn, 100);
    }

    printf("[audio_service] Shutting down...\n");

    alsa_mixer_close(g_am);
    dbus_connection_unref(g_conn);
    
    return 0;
}