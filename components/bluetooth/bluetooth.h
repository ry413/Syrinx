#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum {
    CMD_NONE,
    CMD_GET_VOLUME,
    CMD_GET_TOTAL_FILES,
    CMD_GET_FILE_INDEX,
    CMD_GET_FILE_NAME,
    CMD_NEXT_TRACK,
    CMD_PREV_TRACK,
    CMD_PLAY_PAUSE,
    CMD_AT_AJ
} command_type_t;

extern char **utf8_file_names;
extern int total_files_count;
extern int current_file_index;

void get_all_file_names(void);

// 初始化蓝牙模块
esp_err_t bluetooth_init(void);

// 发送 AT 指令到蓝牙模块
esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type);

// 等待并接收蓝牙模块的响应
esp_err_t bluetooth_wait_for_response(char *response, size_t max_len);

// 蓝牙通信任务
void bluetooth_task(void *pvParameters);

void at_task(void *pvParameters);
#endif // BLUETOOTH_H
