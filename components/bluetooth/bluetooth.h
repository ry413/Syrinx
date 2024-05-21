#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"

// 初始化蓝牙模块
esp_err_t bluetooth_init(void);

// 发送 AT 指令到蓝牙模块
esp_err_t bluetooth_send_at_command(const char *command);

// 等待并接收蓝牙模块的响应
esp_err_t bluetooth_wait_for_response(char *response, size_t max_len);

// 蓝牙通信任务
void bluetooth_task(void *pvParameters);

void at_task(void *pvParameters);
#endif // BLUETOOTH_H
