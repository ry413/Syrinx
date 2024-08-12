#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum {
    CMD_NONE,
    // CMD_GET_VOLUME,             // AT+QA    获取音量
    CMD_GET_DIR_FILE_COUNT,     // AT+M2    获取当前目录下的文件数量
    CMD_CHANGE_DIR,             // AT+M6xx  进入某个目录
    // CMD_GET_CURR_FILE_NAME,     // AT+MF    获取当前音频文件名
    CMD_GET_DIR_FILE_NAMES,      // AT+M4xx  获取当前目录下所有音频文件名
    CMD_NEXT_TRACK,             // AT+CC    下一首
    CMD_PREV_TRACK,             // AT+CD    上一首
    CMD_PLAY_PAUSE_TOGGLE,      // AT+CB    播放/暂停
    CMD_STOP_TRACK,             // AT+AA0   停止播放
    CMD_PLAY_TRACK,             // AT+AA1   播放
    CMD_PAUSE_TRACK,            // AT+AA2   暂停
    CMD_PLAY_MUSIC_WITH_ID,     // AT+AFxx  播放指定序号的歌
    CMD_GET_DURATION,           // AT+MT    获取当前音频时长
    CMD_BLUETOOTH_SET_NAME,     // AT+BD    设置蓝牙名称
    CMD_BLUETOOTH_SET_PASSWORD, // AT+BE    设置蓝牙密码
    CMD_BLUETOOTH_GET_NAME,     // AT+TD    获取蓝牙名称
    CMD_BLUETOOTH_GET_PASSWORD, // AT+TE    获取蓝牙密码
    CMD_BLUETOOTH_GET_STATE,    // AT+TS    获取蓝牙状态
    CMD_REBOOT,                 // AT+CZ    芯片复位
    CMD_SET_VOLUME,             // AT+CA    设置音量
    CMD_CHANGE_TO_BLUETOOTH,    // AT+CM1   切换到蓝牙模式
    CMD_CHANGE_TO_MUSIC,        // AT+CM2   切换到音乐模式
    CMD_CHANGE_TO_IDLE,         // AT_CM0   切换到空闲模式
    CMD_CHANGE_CHANNEL,         // AT+CLx   修改功放通道
    CMD_GET_PLAY_STATE,         // AT+MP    获取播放状态
    CMD_EQUALIZER_SET,          // AT+CQx   设置均衡器
    CMD_GET_DEVICE_STATE,       // AT+MV    查询设备状态
    CMD_CHANGE_PROMPT_TONE,     // AT+CN    修改提示音开关状态
} command_type_t;

extern command_type_t current_command;

extern EventGroupHandle_t music_event_group;        // 音乐播放相关的事件组

#define EVENT_NEXT_TRACK (1 << 0)                   // 已换到下一首
#define EVENT_PREV_TRACK (1 << 1)                   // 已换到上一首
#define EVENT_PLAY_PAUSE_TOGGLE (1 << 2)            // 已播放/暂停
#define EVENT_STOP_TRACK (1 << 3)                   // 已停止
#define EVENT_PLAY_TRACK (1 << 4)                   // 已播放
#define EVENT_PAUSE_TRACK (1 << 5)                  // 已暂停
#define EVENT_PLAY_MUSIC_WITH_ID (1 << 6)           // 已播放指定序号的歌
#define EVENT_GET_DURATION (1 << 7)                // 已获得总时长
#define EVENT_EQUALIZER_SET (1 << 8)               // 已设置均衡器
#define EVENT_GET_PLAY_STATE (1 << 9)              // 已获取播放状态


extern EventGroupHandle_t bt_event_group;           // 除了音乐播放相关的事件组

#define EVENT_GET_DIR_FILE_COUNT (1 << 0)           // 已获得文件数量
#define EVENT_CHANGE_DIR (1 << 1)                   // 已进入某个目录
#define EVENT_FILE_LIST_COMPLETE (1 << 2)           // 已获取完文件名列表
#define EVENT_SET_VOLUME (1 << 3)                   // 已设置音量
#define EVENT_BLUETOOTH_SET_PASSWORD (1 << 4)       // 已设置蓝牙密码
#define EVENT_BLUETOOTH_GET_NAME (1 << 5)           // 已获取蓝牙名称
#define EVENT_BLUETOOTH_GET_PASSWORD (1 << 6)       // 已获取蓝牙密码
#define EVENT_BLUETOOTH_GET_STATE (1 << 7)          // 已获取蓝牙状态
#define EVENT_CHANGE_TO_BLUETOOTH (1 << 8)          // 已换到蓝牙模式
#define EVENT_CHANGE_TO_MUSIC (1 << 9)              // 已换到音乐模式
#define EVENT_CHANGE_TO_IDLE (1 << 10)              // 已换到空闲模式
#define EVENT_CHANGE_CHANNEL (1 << 11)              // 已修改功放通道
#define EVENT_BLUETOOTH_CONNECTED (1 << 12)         // 蓝牙已连接
#define EVENT_BLUETOOTH_DISCONNECTED (1 << 13)      // 蓝牙已断开
#define EVENT_END_PLAY (1 << 14)                    // 播放已结束
#define EVENT_STARTUP_SUCCESS (1 << 15)             // 模块已复位
#define EVENT_GET_DIR_FILE_NAMES (1 << 16)          // 已获取当前目录下所有音频文件名
#define EVENT_ALL_DURATION_COMPLETE (1 << 17)       // 已获取完音乐库每首歌的总时长
#define EVENT_GET_DEVICE_STATE (1 << 18)            // 已获取设备状态
#define EVENT_CHANGE_PROMPT_TONE (1 << 19)          // 已修改提示音开关状态




// 感觉, 大概都是别的组件需要的东西, 在这里声明, 好收到返回值后可以设上, 还能被别的组件引用
extern char **utf8_file_names;
extern uint32_t music_files_count;
extern uint32_t bath_files_count;
extern int *bath_file_ids;
extern int current_dir_files_count;

extern int current_playing_index;
extern uint32_t current_music_duration;
extern char bluetooth_name[13];
extern char bluetooth_password[5];
extern int bluetooth_state;
extern int work_mode;
extern int play_state;

extern int device_state;

extern uint32_t bath_channel_bit;   // 浴室功放通道, 代表AT+CL%d

extern char final_version[50];


void get_all_file_names(void);
void get_all_music_duration(void);
esp_err_t bluetooth_init(void);
esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type);

esp_err_t bluetooth_wait_for_response(char *response, size_t max_len);
void bluetooth_monitor_task(void *pvParameters);

void open_living_room_channel(void);
void open_bath_channel(void);

void show_error_info(void);

void AT_MV(void);
void AT_CL(int channel);
void AT_CM(int mode);
void AT_M6(int dir_id);
void AT_M4(int m4_targer);
void AT_M2(void);
void AT_MT(void);
void AT_CC(void);
void AT_AF(int music_id);
void AT_CN(int cmd);
void AT_AA0(void);
void AT_TD(void);
void AT_TE(void);
void AT_BD(char *name);
void AT_BE(char *pass);
void AT_CA(int volume);
void AT_CQ(int equalizer);
void AT_CB(void);

#endif // BLUETOOTH_H
