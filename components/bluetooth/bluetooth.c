#include "bluetooth.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <string.h>
#include <iconv.h>
#include <locale.h>
#include <wchar.h>


#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        GPIO_NUM_43
#define UART_RX_PIN        GPIO_NUM_44
#define BUF_SIZE           1024
#define QUEUE_SIZE         10
#define MAX_FILES          100

static const char *TAG = "Bluetooth";
static QueueHandle_t uart_queue;
static EventGroupHandle_t event_group;

bool is_music_mode = true;

char **utf8_file_names = NULL;  // 储存文件名的数组
int total_files_count = 0;      // 这两个都是在初始化时获取全部文件名用的
int current_file_index = 0; 
int current_playing_index = 0;  // 播放阶段使用, 当前在放的音频, 在utf8_file_names中的索引, 我也不知道为什么要定义在这里
int current_music_duration = 0;


command_type_t current_command = CMD_NONE;

EventGroupHandle_t get_bluetooth_event_group(void) {
    return event_group;
}

int utf16_to_utf8(const unsigned short* utf16_str, char** utf8_str) {
    if (utf16_str == NULL || utf8_str == NULL) return 0;

    int utf8_index = 0;
    int utf8_size = 1; // 为字符串结束'\0'留出空间
    int i = 0;

    while (utf16_str[i] != '\0') {
        unsigned int unicode_code = utf16_str[i++];

        if (unicode_code >= 0xD800 && unicode_code <= 0xDBFF) { // 判断是否为代理对
            unsigned int surrogate_pair_code = utf16_str[i];
            if (surrogate_pair_code >= 0xDC00 && surrogate_pair_code <= 0xDFFF) {
                unicode_code = ((unicode_code - 0xD800) << 10) + (surrogate_pair_code - 0xDC00) + 0x10000;
                i++; // 跳过低位代理对
            }
        }

        if (unicode_code < 0x80) utf8_size += 1;
        else if (unicode_code < 0x800) utf8_size += 2;
        else if (unicode_code < 0x10000) utf8_size += 3;
        else utf8_size += 4;
    }

    *utf8_str = (char*) malloc(utf8_size * sizeof(char));
    if (*utf8_str == NULL) return 0;

    utf8_index = 0;
    i = 0;

    while (utf16_str[i] != '\0') {
        unsigned int unicode_code = utf16_str[i++];

        if (unicode_code >= 0xD800 && unicode_code <= 0xDBFF) {
            unsigned int surrogate_pair_code = utf16_str[i];
            if (surrogate_pair_code >= 0xDC00 && surrogate_pair_code <= 0xDFFF) {
                unicode_code = ((unicode_code - 0xD800) << 10) + (surrogate_pair_code - 0xDC00) + 0x10000;
                i++;
            }
        }

        if (unicode_code < 0x80) {
            (*utf8_str)[utf8_index++] = unicode_code;
        } else if (unicode_code < 0x800) {
            (*utf8_str)[utf8_index++] = ((unicode_code >> 6) & 0x1F) | 0xC0;
            (*utf8_str)[utf8_index++] = (unicode_code & 0x3F) | 0x80;
        } else if (unicode_code < 0x10000) {
            (*utf8_str)[utf8_index++] = ((unicode_code >> 12) & 0x0F) | 0xE0;
            (*utf8_str)[utf8_index++] = ((unicode_code >> 6) & 0x3F) | 0x80;
            (*utf8_str)[utf8_index++] = (unicode_code & 0x3F) | 0x80;
        } else {
            (*utf8_str)[utf8_index++] = ((unicode_code >> 18) & 0x07) | 0xF0;
            (*utf8_str)[utf8_index++] = ((unicode_code >> 12) & 0x3F) | 0x80;
            (*utf8_str)[utf8_index++] = ((unicode_code >> 6) & 0x3F) | 0x80;
            (*utf8_str)[utf8_index++] = (unicode_code & 0x3F) | 0x80;
        }
    }

    (*utf8_str)[utf8_index] = '\0';
    return 1;
}

esp_err_t bluetooth_init(void) {
    // 初始化UART 0
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, BUF_SIZE * 2, QUEUE_SIZE, &uart_queue, intr_alloc_flags));

    event_group = xEventGroupCreate();
    
    ESP_LOGI(TAG, "UART 0 初始化成功");
    return ESP_OK;
}

esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type) {
    // 更新当前命令类型
    current_command = cmd_type;

    // 创建带有换行符的命令
    char command_with_newline[128];
    snprintf(command_with_newline, sizeof(command_with_newline), "%s\r\n", command);

    // 发送命令
    int len = uart_write_bytes(UART_PORT_NUM, command_with_newline, strlen(command_with_newline));
    if (len < 0) {
        ESP_LOGE(TAG, "UART 0 write error");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Send AT command: %s", command);
        return ESP_OK;
    }
}

// // 获取音量的处理任务
// void handle_volume_response_task(void *pvParameters) {
//     // 等待事件组中的事件
//     EventBits_t bits = xEventGroupWaitBits(event_group, EVENT_VOLUME_RESPONSE, pdTRUE, pdFALSE, portMAX_DELAY);

//     if (bits & EVENT_VOLUME_RESPONSE) {
//         // 处理响应
//         ESP_LOGI(TAG, "Volume info: %s", volume_response);
//     } else {
//         ESP_LOGE(TAG, "Failed to get volume info");
//     }

//     // 删除任务
//     vTaskDelete(NULL);
// }

// void send_get_volume_command(void) {
//     bluetooth_send_at_command("AT+QA");
//     xTaskCreate(handle_volume_response_task, "Volume Response Task", 4096, NULL, 5, NULL);
// }

// 添加文件名到列表
void add_file_name(char **utf8_file_names, int index, const char *file_name) {
    // 确保 file_name 以 "MF+" 开头并以 ".mp3" 结尾, 理想中显然这不应该出错, 谁知道呢
    if (strncmp(file_name, "MF+", 3) != 0 || strcmp(file_name + strlen(file_name) - 4, ".mp3") != 0) {
        ESP_LOGE(TAG,"Invalid file name format: %s", file_name);
        return;
    }

    // 跳过 "MF+" 前缀
    const char *name_start = file_name + 3;

    // 计算新的文件名长度（去除 ".mp3" 的长度）
    size_t name_length = strlen(name_start) - 4;

    // 创建临时缓冲区以存储处理后的文件名
    char temp_name[name_length + 1];
    strncpy(temp_name, name_start, name_length);
    temp_name[name_length] = '\0';

    utf8_file_names[index] = strdup(temp_name);
    if (utf8_file_names[index] == NULL) {
        ESP_LOGE(TAG, "Failed to duplicate string");
        return;
    };
}
void get_all_file_names(void) {
    // 开机时会收到一个ERR+1, 所以这里延迟一秒等那个ERR+1被读掉
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // 发送获取总文件数的命令
    bluetooth_send_at_command("AT+M2", CMD_GET_TOTAL_FILES);
    EventBits_t bits = xEventGroupWaitBits(event_group, EVENT_TOTAL_FILES, pdTRUE, pdFALSE, portMAX_DELAY);

    // 总文件数
    if (bits & EVENT_TOTAL_FILES) {
        ESP_LOGI(TAG, "Total files: %d", total_files_count);
        
        utf8_file_names = (char **)malloc(total_files_count * sizeof(char *));

        if ((utf8_file_names == NULL)) {
            ESP_LOGE(TAG, "Failed to allocate memory for file_namess");
            return;
        }
        for (int i = 0; i < total_files_count; i++) {
            utf8_file_names[i] = (char *)malloc(100 * sizeof(char));
            // 如果有哪个分配失败, 把之前已分配的释放掉
            if (utf8_file_names[i] == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for utf8_file_names[%d]", i);
                for (int j = 0; j < i; j++) {
                    free(utf8_file_names[j]);
                }
                free(utf8_file_names);
                return;
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to get total file count");
        return;
    }
    
    // 遍历所有文件
    for (int i = 1; i <= total_files_count; ++i) {
        // 发送获取当前文件序号的命令
        bluetooth_send_at_command("AT+M1", CMD_GET_FILE_INDEX);
        bits = xEventGroupWaitBits(event_group, EVENT_FILE_INDEX, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & EVENT_FILE_INDEX) {
            // 解析当前文件序号
            ESP_LOGI(TAG, "Current file index: %d", current_file_index);

            // 发送获取当前文件名的命令
            bluetooth_send_at_command("AT+MF", CMD_GET_FILE_NAME);
            bits = xEventGroupWaitBits(event_group, EVENT_FILE_NAME, pdTRUE, pdFALSE, portMAX_DELAY);

            if (bits & EVENT_FILE_NAME) {
                // 大耦合了, 读取文件名后储存到file_names的逻辑在bluetooth_wait_for_response里


                // char* utf8_str = NULL;
                // utf16_to_utf8(utf16_file_names[current_file_index], &utf8_str);
                // printf("UTF8编码的字符串: %s\n", utf8_str);
                // add_file_name(utf8_file_names, current_file_index - 1, utf8_str);
                // free(utf8_str);

                // sscanf(response_buffer, "MF+%s", file_names[current_file_index - 1]);
                // add_file_name()
                // ESP_LOGI(TAG, "File name: %s", file_names[current_file_index - 1]);
            } else {
                ESP_LOGE(TAG, "Failed to get file name for file index %d", current_file_index);
                return;
            }
        } else {
            ESP_LOGE(TAG, "Failed to get file index %d", i);
            return;
        }

        // 如果不是最后一个文件，发送下一曲命令
        if (i < total_files_count) {
            bluetooth_send_at_command("AT+CC", CMD_NEXT_TRACK);
            bits = xEventGroupWaitBits(event_group, EVENT_NEXT_TRACK, pdTRUE, pdFALSE, portMAX_DELAY);
            if (!(bits & EVENT_NEXT_TRACK)) {
                ESP_LOGE(TAG, "Failed to move to next track");
                return;
            }

        }
    }

    // 打印所有文件名
    for (int i = 0; i < total_files_count; ++i) {
        ESP_LOGI(TAG, "File %d: %s", i + 1, utf8_file_names[i]);
    }

    bluetooth_send_at_command("AT+CB", CMD_PLAY_PAUSE);
}
esp_err_t bluetooth_wait_for_response(char *response, size_t max_len) {
    uart_event_t event;

    while (1) {
        // 等待 UART 事件
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                int total_len = 0;
                int len;

                while (total_len < max_len - 1) {
                    len = uart_read_bytes(UART_PORT_NUM, (uint8_t *)(response + total_len), max_len - 1 - total_len, 1000 / portTICK_PERIOD_MS);
                    if (len > 0) {
                        // AT+MF会返回utf16, 需要特殊处理, 希望其他指令不用
                        if (strstr(response, "MF+") != NULL) {
                            // 更新总长度
                            total_len += len;
                            unsigned short utf16_str[max_len / 2]; // 用于存储UTF-16编码的字符串
                            // 处理前三个单字节 MF+
                            utf16_str[0] = response[0];
                            utf16_str[1] = response[1];
                            utf16_str[2] = response[2];

                            // 处理剩余的字节
                            for (int i = 3, j = 3; i < len / 2 + 1; i++, j+=2) {
                                // 大端转小端
                                utf16_str[i] = response[j] | (response[j + 1] << 8);
                            }
                            utf16_str[len / 2 + 1] = '\0'; // 确保数组末尾有终止符

                            char *utf8 = NULL;
                            utf16_to_utf8(utf16_str, &utf8);
                            printf("UTF8编码的字符串: %s\n", utf8);
                            add_file_name(utf8_file_names, current_file_index - 1, utf8);
                            free(utf8);
                            // 这里直接return了, 因为MF+的处理太过特殊, 就不让后面的继续处理了
                            return ESP_OK;
                        } else {
                            // 消除空字节
                            int valid_len = 0;
                            for (int i = 0; i < len; i++) {
                                if (response[total_len + i] != '\0') {
                                    response[total_len + valid_len] = response[total_len + i];
                                    valid_len++;
                                }
                            }
                            total_len += valid_len;
                            response[total_len] = '\0';

                            // for (int i = 0; i < valid_len; i++) {
                            //     ESP_LOGI(TAG, "Received byte: 0x%02X", response[total_len - valid_len + i]);
                            // }
                        }

                        // Check for end of response (assuming newline character marks the end)
                        if (strchr(response, '\n') != NULL) {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                if (total_len > 0) {
                    response[total_len] = '\0';
                    ESP_LOGI(TAG, "Received full response: %s", response);
                    return ESP_OK;
                } else {
                    ESP_LOGE(TAG, "No data received");
                    return ESP_FAIL;
                }
            } else if (event.type == UART_FIFO_OVF) {
                ESP_LOGW(TAG, "UART FIFO Overflow");
                uart_flush_input(UART_PORT_NUM);
                xQueueReset(uart_queue);
            } else if (event.type == UART_BUFFER_FULL) {
                ESP_LOGW(TAG, "UART Buffer Full");
                uart_flush_input(UART_PORT_NUM);
                xQueueReset(uart_queue);
            } else if (event.type == UART_BREAK) {
                ESP_LOGW(TAG, "UART Break");
            } else if (event.type == UART_PARITY_ERR) {
                ESP_LOGW(TAG, "UART Parity Error");
            } else if (event.type == UART_FRAME_ERR) {
                ESP_LOGW(TAG, "UART Frame Error");
            } else {
                ESP_LOGW(TAG, "UART Other??");
            }
        }
    }
}

void bluetooth_monitor_task(void *pvParameters) {
    char response[BUF_SIZE];
    while (1) {
        // 等待并接收响应
        if (bluetooth_wait_for_response(response, sizeof(response)) == ESP_OK) {
            ESP_LOGI(TAG, "Received response: cmd: %d, %s\n", current_command, response);
            // 只有响应OK才需要判断是给谁的OK
            if (strncmp(response, "OK", 2) == 0) {
                // 暂时没用的就不设置事件组了
                switch (current_command) {
                    case CMD_NEXT_TRACK:
                        xEventGroupSetBits(event_group, EVENT_NEXT_TRACK);
                        break;
                    case CMD_PREV_TRACK:
                        // xEventGroupSetBits(event_group, EVENT_PREV_TRACK);
                        break;
                    case CMD_PLAY_PAUSE:
                        xEventGroupSetBits(event_group, EVENT_PLAY_PAUSE);
                        break;
                    case CMD_STOP_STATE:
                        xEventGroupSetBits(event_group, EVENT_STOP_STATE);
                        break;
                    case CMD_PLAY_STATE:
                        // xEventGroupSetBits(event_group, EVENT_PLAY_STATE);
                        break;
                    case CMD_PAUSE_STATE:
                        // xEventGroupSetBits(event_group, EVENT_PAUSE_STATE);
                        break;
                    case CMD_PLAY_MUSIC:
                        xEventGroupSetBits(event_group, EVENT_PLAY_MUSIC);
                        break;
                    case CMD_SET_VOLUME:
                        xEventGroupSetBits(event_group, EVENT_SET_VOLUME);
                        break;
                    case CMD_BLUETOOTH_NAME:
                        xEventGroupSetBits(event_group, EVENT_BLUETOOTH_NAME);
                        break;
                    case CMD_BLUETOOTH_PASSWORD:
                        xEventGroupSetBits(event_group, EVENT_BLUETOOTH_PASSWORD);
                        break;
                    case CMD_DISCONNECT_BLUETOOTH:
                        xEventGroupSetBits(event_group, EVENT_DISCONNECT_BLUETOOTH);
                        break;
                    case CMD_ON_MUTE:
                        xEventGroupSetBits(event_group, EVENT_ON_MUTE);
                        break;
                    case CMD_OFF_MUTE:
                        xEventGroupSetBits(event_group, EVENT_OFF_MUTE);
                        break;
                    default:
                        ESP_LOGI(TAG, "Other CMD: %d", current_command);
                }
            } else if (strncmp(response, "QA+", 3) == 0) {
                ESP_LOGI(TAG, "Volume: %s", response);
                // strncpy(volume_response, response, BUF_SIZE);
                // xEventGroupSetBits(event_group, EVENT_VOLUME_RESPONSE);
            } else if (strncmp(response, "M2+", 3) == 0) {
                sscanf(response, "M2+%d", &total_files_count);
                xEventGroupSetBits(event_group, EVENT_TOTAL_FILES);
            } else if (strncmp(response, "M1+", 3) == 0) {
                sscanf(response, "M1+%d", &current_file_index);
                xEventGroupSetBits(event_group, EVENT_FILE_INDEX);
            } else if (strncmp(response, "MF+", 3) == 0) {
                // 另一个耦合, 那边传来文件名后, 直接在wait_for_response那当场处理完储存了
                xEventGroupSetBits(event_group, EVENT_FILE_NAME);
            } else if (strncmp(response, "MT+", 3) == 0) {
                sscanf(response, "MT+%d", &current_music_duration);
                xEventGroupSetBits(event_group, EVENT_DURATION);
            } else if (strncmp(response, "TS+", 3) == 0) {
                int bl_state;
                sscanf(response, "TS+%d", &bl_state);
                // 1是等待连接
                if (bl_state == 1) {
                    xEventGroupSetBits(event_group, EVENT_BLUETOOTH_WAIT_FOR_CONNET);
                }
                // 0表示处于音乐模式, TS指令只在蓝牙模式发, 所以应该不可能出现(所以丢掉了), 如果是剩下的就表示已接通
                else if (bl_state != 0) {
                    xEventGroupSetBits(event_group, EVENT_BLUETOOTH_CONNECTED);
                }
            }
            // AT+CZ复位芯片时会发个0xF0
            else if (response[0] == 0xF0) {
                printf("REBOOTED\n");
            }
            
        } else {
            ESP_LOGE(TAG, "Failed to get response");
        }
    }
}

