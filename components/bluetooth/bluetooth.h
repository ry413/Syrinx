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
    CMD_PLAY_MUSIC,
    CMD_GET_DURATION,
    CMD_REBOOT
} command_type_t;

#define EVENT_TOTAL_FILES (1 << 0)
#define EVENT_FILE_INDEX (1 << 1)
#define EVENT_FILE_NAME (1 << 2)
#define EVENT_NEXT_TRACK (1 << 3)
#define EVENT_PREV_TRACK (1 << 4)
#define EVENT_PLAY_PAUSE (1 << 5)
#define EVENT_PLAY_MUSIC (1 << 6)
#define EVENT_DURATION (1 << 7)


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
void bluetooth_task(void *pvParameters);

#endif // BLUETOOTH_H
