/*
 * ipc_base.c — D-Bus 客户端公共基础设施
 */
#include "ipc_base.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

DBusConnection *ipc_base_connect(const char *bus_address)
{
    DBusError err;
    dbus_error_init(&err);

    DBusConnection *conn = dbus_connection_open(bus_address, &err);
    if (!conn || dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_base] connection failed: %s\n", err.message);
        dbus_error_free(&err);
        return NULL;
    }

    dbus_bus_register(conn, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_base] register failed: %s\n", err.message);
        dbus_error_free(&err);
        dbus_connection_unref(conn);
        return NULL;
    }

    dbus_connection_set_exit_on_disconnect(conn, FALSE);
    return conn;
}

int ipc_base_wait_for_service(DBusConnection *conn,
                              const char *svc_name, int max_retries)
{
    for (int i = 0; i < max_retries; i++) {
        DBusMessage *m = dbus_message_new_method_call(
            "org.freedesktop.DBus", "/org/freedesktop/DBus",
            "org.freedesktop.DBus", "NameHasOwner");
        if (!m) return -1;

        const char *name = svc_name;
        dbus_message_append_args(m, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);

        DBusError e;
        dbus_error_init(&e);
        DBusMessage *r = dbus_connection_send_with_reply_and_block(conn, m, 50, &e);
        if (r) {
            dbus_bool_t has_owner = FALSE;
            dbus_message_get_args(r, &e, DBUS_TYPE_BOOLEAN, &has_owner, DBUS_TYPE_INVALID);
            dbus_message_unref(r);
            dbus_error_free(&e);
            dbus_message_unref(m);
            if (has_owner) return 0;
        } else {
            dbus_error_free(&e);
            dbus_message_unref(m);
        }
        usleep(20000);
    }
    return -1;
}

int ipc_base_watch_service(DBusConnection *conn, const char *svc_name)
{
    char rule[256];
    snprintf(rule, sizeof(rule),
             "type='signal',interface='org.freedesktop.DBus',"
             "member='NameOwnerChanged',arg0='%s'", svc_name);

    DBusError err;
    dbus_error_init(&err);
    dbus_bus_add_match(conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "[ipc_base] add_match NameOwnerChanged(%s) failed: %s\n",
                svc_name, err.message);
        dbus_error_free(&err);
        return -1;
    }
    return 0;
}

void ipc_base_disconnect(DBusConnection **conn,
                         DBusHandleMessageFunction filter)
{
    if (!conn || !*conn) return;

    if (filter)
        dbus_connection_remove_filter(*conn, filter, NULL);

    dbus_connection_unref(*conn);
    *conn = NULL;
}