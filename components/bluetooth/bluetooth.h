#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum {
    CMD_NONE,
    CMD_GET_VOLUME,             // AT+QA    获取音量
    CMD_GET_TOTAL_FILES,        // AT+M2    获取音频数
    CMD_CHANGE_DIR,             // AT+M6xx  进入某个目录
    CMD_GET_CURR_FILE_NAME,     // AT+MF    获取当前音频文件名
    CMD_GET_ALL_FILE_NAME,      // AT+M4xx  获取当前目录下所有音频文件名
    CMD_NEXT_TRACK,             // AT+CC    下一首
    CMD_PREV_TRACK,             // AT+CD    上一首
    CMD_PLAY_PAUSE,             // AT+CB    播放/暂停
    CMD_STOP_STATE,             // AT+AA0   停止播放
    CMD_PLAY_STATE,             // AT+AA1   播放
    CMD_PAUSE_STATE,            // AT+CB    暂停
    CMD_PLAY_MUSIC,             // AT+AFxx  播放指定序号的歌
    CMD_GET_DURATION,           // AT+MT    获取当前音频时长
    CMD_BLUETOOTH_SET_NAME,     // AT+BD    设置蓝牙名称
    CMD_BLUETOOTH_SET_PASSWORD, // AT+BE    设置蓝牙密码
    CMD_BLUETOOTH_GET_NAME,     // AT+TD    获取蓝牙名称
    CMD_BLUETOOTH_GET_PASSWORD, // AT+TE    获取蓝牙密码
    CMD_REBOOT,                 // AT+CZ    芯片复位
    // CMD_DISCONNECT_BLUETOOTH,   // AT+BA1   断开蓝牙连接
    CMD_SET_VOLUME,             // AT+CA    设置音量
    CMD_CHANGE_TO_BLUETOOTH,    // AT+CM1   切换到蓝牙模式
    CMD_CHANGE_TO_MUSIC,        // AT+CM2   切换到音乐模式
    CMD_ON_MUTE,                // AT+CL0   关闭功放
    CMD_OFF_MUTE,               // AT+CL3   打开功放
    // CMD_BLUETOOTH_STATE,         // AT+TS    获取蓝牙状态
    CMD_EQUALIZER_SET,          // AT+CQx   设置均衡器
} command_type_t;

#define EVENT_TOTAL_FILES_COUNT (1 << 0)            // 已获得文件数量
#define EVENT_CHANGE_DIR (1 << 1)                   // 已进入某个目录
#define EVENT_FILE_LIST_COMPLETE (1 << 2)           // 已获取完文件名列表
#define EVENT_NEXT_TRACK (1 << 3)                   // 已换到下一首
#define EVENT_PREV_TRACK (1 << 4)                   // 已换到上一首
#define EVENT_PLAY_PAUSE (1 << 5)                   // 已播放/暂停
#define EVENT_STOP_STATE (1 << 6)                   // 已停止
#define EVENT_PLAY_STATE (1 << 7)                   // 已播放
#define EVENT_PAUSE_STATE (1 << 8)                  // 已暂停
#define EVENT_PLAY_MUSIC (1 << 9)                   // 已播放指定序号的歌
#define EVENT_DURATION (1 << 10)                    // 已获得总时长
#define EVENT_SET_VOLUME (1 << 11)                  // 已设置音量
#define EVENT_BLUETOOTH_SET_PASSWORD (1 << 12)      // 已设置蓝牙密码
#define EVENT_BLUETOOTH_GET_NAME (1 << 13)          // 已获取蓝牙名称
#define EVENT_BLUETOOTH_GET_PASSWORD (1 << 14)      // 已获取蓝牙密码
#define EVENT_CHANGE_TO_BLUETOOTH (1 << 15)         // 已换到蓝牙模式
#define EVENT_CHANGE_TO_MUSIC (1 << 16)             // 已换到音乐模式
#define EVENT_ON_MUTE (1 << 17)                     // 已关闭功放
#define EVENT_OFF_MUTE (1 << 18)                    // 已打开功放
#define EVENT_BLUETOOTH_CONNECTED (1 << 19)         // 蓝牙已连接
#define EVENT_BLUETOOTH_DISCONNECTED (1 << 20)      // 蓝牙已断开
#define EVENT_END_PLAY (1 << 21)                    // 播放已结束
#define EVENT_EQUALIZER_SET (1 << 22)               // 已设置均衡器
#define EVENT_STARTUP_SUCCESS (1 << 23)             // 模块已上电并丢掉了返回值



extern command_type_t current_command;
extern EventGroupHandle_t bt_event_group;

// 很诡异
extern char **utf8_file_names;
extern int total_files_count;
extern int current_playing_index;
extern int current_music_duration;
extern char bluetooth_name[13];
extern char bluetooth_password[5];


void get_all_file_names(void);

esp_err_t bluetooth_init(void);
esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type);

esp_err_t bluetooth_wait_for_response(char *response, size_t max_len);
void bluetooth_monitor_task(void *pvParameters);

#endif // BLUETOOTH_H
