#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum {
    CMD_NONE,
    CMD_GET_VOLUME,             // AT+QA    获取音量
    CMD_GET_TOTAL_FILES,        // AT+M2    获取音频数
    CMD_GET_FILE_INDEX,         // AT+M1    获取当前音频索引
    CMD_GET_FILE_NAME,          // AT+MF    获取当前音频文件名
    CMD_NEXT_TRACK,             // AT+CC    下一首
    CMD_PREV_TRACK,             // AT+CD    上一首
    CMD_PLAY_PAUSE,             // AT+CB    播放/暂停
    CMD_STOP_STATE,             // AT+AA0   停止播放
    CMD_PLAY_STATE,             // AT+AA1   播放
    CMD_PAUSE_STATE,            // AT+AA2   暂停
    CMD_PLAY_MUSIC,             // AT+AJ    播放指定文件名
    CMD_GET_DURATION,           // AT+MT    获取当前音频时长
    CMD_BLUETOOTH_NAME,         // AT+BD    设置蓝牙名称
    CMD_BLUETOOTH_PASSWORD,     // AT+BE    设置蓝牙密码
    CMD_REBOOT,                 // AT+CZ    芯片复位
    CMD_DISCONNECT_BLUETOOTH,   // AT+BA1   断开蓝牙连接
    CMD_SET_VOLUME,             // AT+CA    设置音量
    CMD_CHANGE_TO_BLUETOOTH,    // AT+CH    切换到蓝牙模式
    CMD_CHANGE_TO_MUSIC,        // AT+CM    切换到音乐模式
    CMD_ON_MUTE,                // AT+CU1   静音
    CMD_OFF_MUTE,               // AT+CU0   取消静音
} command_type_t;

#define EVENT_TOTAL_FILES (1 << 0)
#define EVENT_FILE_INDEX (1 << 1)
#define EVENT_FILE_NAME (1 << 2)
#define EVENT_NEXT_TRACK (1 << 3)
#define EVENT_PREV_TRACK (1 << 4)
#define EVENT_PLAY_PAUSE (1 << 5)
#define EVENT_STOP_STATE (1 << 6)
#define EVENT_PLAY_STATE (1 << 7)
#define EVENT_PAUSE_STATE (1 << 8)
#define EVENT_PLAY_MUSIC (1 << 9)
#define EVENT_DURATION (1 << 10)
#define EVENT_BLUETOOTH_NAME (1 << 11)
#define EVENT_BLUETOOTH_PASSWORD (1 << 12)
#define EVENT_DISCONNECT_BLUETOOTH (1 << 13)
#define EVENT_SET_VOLUME (1 << 14)
// #define EVENT_CHANGE_TO_BLUETOOTH (1 << 15)
// #define EVENT_CHANGE_TO_MUSIC (1 << 16)
#define EVENT_ON_MUTE (1 << 17)
#define EVENT_OFF_MUTE (1 << 18)

extern bool is_music_mode;

extern command_type_t current_command;

// 开机获取音频名列表用的变量, 不知道给谁引用了
extern char **utf8_file_names;
extern int total_files_count;
extern int current_file_index;
extern int current_playing_index;
extern int current_music_duration;

EventGroupHandle_t get_bluetooth_event_group(void);
void get_all_file_names(void);

esp_err_t bluetooth_init(void);
esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type);

esp_err_t bluetooth_wait_for_response(char *response, size_t max_len);
void bluetooth_monitor_task(void *pvParameters);

#endif // BLUETOOTH_H
