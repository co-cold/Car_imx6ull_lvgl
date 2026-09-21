#ifndef IPC_BASE_H
#define IPC_BASE_H

#include <dbus/dbus.h>

/*
 * ipc_base.h — D-Bus 客户端公共基础设施
 *
 * 消除各 IPC 模块中重复的连接初始化、服务发现和资源清理代码。
 */

// 打开 D-Bus 连接 → 注册 → 禁用退出时断开。
// 返回 NULL 表示失败，调用者持有返回的连接。
DBusConnection *ipc_base_connect(const char *bus_address);

// 轮询 NameHasOwner 等待服务名注册（每次 20ms）。
// max_retries 次后超时返回 -1，成功返回 0。
int ipc_base_wait_for_service(DBusConnection *conn,
                              const char *svc_name, int max_retries);

// 订阅指定服务名的 NameOwnerChanged 信号，用于重连检测。
// 成功返回 0，失败返回 -1（非致命，仅日志警告）。
int ipc_base_watch_service(DBusConnection *conn, const char *svc_name);

// 清理：移除 filter + unref 连接，指针置空。
void ipc_base_disconnect(DBusConnection **conn,
                         DBusHandleMessageFunction filter);

#endif