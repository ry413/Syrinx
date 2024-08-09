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
#include "ctype.h"
#include "nvs.h"
#include "../../ui/ui.h"


#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        GPIO_NUM_43
#define UART_RX_PIN        GPIO_NUM_44
#define BUF_SIZE           1024
#define QUEUE_SIZE         10
#define MAX_FILES          100

static const char *TAG = "Bluetooth";
static QueueHandle_t uart_queue;
EventGroupHandle_t bt_event_group = NULL;
EventGroupHandle_t music_event_group = NULL;



char **temp_file_names = NULL;  // 储存文件名的数组
int current_dir_files_count = 0;// M2的返回值临时接收变量
uint32_t music_files_count = 0;      // music目录下的文件数
uint32_t bath_files_count = 0;       // bath目录下的文件数
int *bath_file_ids = NULL;      // bath目录下的文件们的id数组
int current_playing_index = 0;  // 播放阶段使用, 当前在放的音频, 在file_names中的索引, 我也不知道为什么要定义在这里
uint32_t current_music_duration = 0;
char bluetooth_name[13];
char bluetooth_password[5];
int bluetooth_state = 0;        // 蓝牙状态
int work_mode = 0;              // 工作模式, 0: 空闲, 1: 蓝牙, 2: 音乐
int play_state = 0;
int device_state = 0;

command_type_t current_command = CMD_NONE;

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

    bt_event_group = xEventGroupCreate();
    music_event_group = xEventGroupCreate();
    
    ESP_LOGI(TAG, "UART 0 初始化成功");
    return ESP_OK;
}

esp_err_t bluetooth_send_at_command(const char *command, command_type_t cmd_type) {
    // 更新当前命令类型
    current_command = cmd_type;

    // 创建带有换行符的命令
    char command_with_newline[20];
    snprintf(command_with_newline, sizeof(command_with_newline), "%s\r\n", command);

    // 发送命令
    int len = uart_write_bytes(UART_PORT_NUM, command_with_newline, strlen(command_with_newline));
    if (len < 0) {
        ESP_LOGE(TAG, "UART 0 write error");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "已发送指令: %s", command);
        return ESP_OK;
    }
}

// 添加文件名到列表
void add_file_name(char **temp_file_names, const char *file_name) {
    // 确保 file_name 以 2位数字 开头并以 ".mp3" 结尾
    if (!isdigit((unsigned char)file_name[0]) || !isdigit((unsigned char)file_name[1]) || strcmp(file_name + strlen(file_name) - 4, ".mp3") != 0) {
        ESP_LOGE(TAG, "Invalid file name format: %s", file_name);
        return;
    }

    // 计算新的文件名长度（去除 ".mp3" 的长度）
    size_t name_length = strlen(file_name) - 4;
    
    // 检查 name_length 是否合理, 极端案例是"00.mp3"
    if (name_length <= 0) {
        ESP_LOGE(TAG, "Invalid file name length after processing: %s", file_name);
        return;
    }
    static int idx = 0;
    // 动态分配并复制处理后的文件名到 temp_file_names 列表中
    temp_file_names[idx] = strndup(file_name, name_length);
    if (temp_file_names[idx] == NULL) {
        ESP_LOGE(TAG, "Failed to duplicate string for temp_file_names[%d]", idx);
        return;
    }
    idx++;
}
// 读取所有音乐文件名并储存于nvs中
void get_all_file_names(void) {
    // 切换到音乐模式
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (1/12)");
    bluetooth_send_at_command("AT+CM2", CMD_CHANGE_TO_MUSIC);
    xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_MUSIC, pdTRUE, pdFALSE, portMAX_DELAY);
    work_mode = 2;
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (2/12)");
    // 必须进入目录才能保证得到对的M2数值
    bluetooth_send_at_command("AT+M602", CMD_CHANGE_DIR);
    xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_DIR, pdTRUE, pdFALSE, portMAX_DELAY);
    bluetooth_send_at_command("AT+M2", CMD_GET_DIR_FILE_COUNT);
    xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_COUNT, pdTRUE, pdFALSE, portMAX_DELAY);
    music_files_count = current_dir_files_count;

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (3/12)");
    // 再弄个bath目录的文件数
    bluetooth_send_at_command("AT+M601", CMD_CHANGE_DIR);
    xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_DIR, pdTRUE, pdFALSE, portMAX_DELAY);
    bluetooth_send_at_command("AT+M2", CMD_GET_DIR_FILE_COUNT);
    xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_COUNT, pdTRUE, pdFALSE, portMAX_DELAY);
    bath_files_count = current_dir_files_count;

    printf("music: %ld, bath: %ld\n", music_files_count, bath_files_count);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (4/12)");
    // 储存文件数
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("filenames", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("get_all_file_names", "Failed to open NVS");
        return;
    }

    err = nvs_set_u32(nvs_handle, "music_count", music_files_count);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return;
    }
    err = nvs_set_u32(nvs_handle, "bath_count", bath_files_count);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return;
    }

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (5/12)");
    // 分配内存, 这里面将分别装music, bath, theme三个目录的文件名, theme(4首)就是自然之音
    temp_file_names = (char **)malloc((music_files_count + bath_files_count + 4) * sizeof(char *));
    if ((temp_file_names == NULL)) {
        ESP_LOGE(TAG, "Failed to allocate memory for file_namess");
        return;
    }
    for (int i = 0; i < (music_files_count + bath_files_count + 4); i++) {
        temp_file_names[i] = (char *)malloc(100 * sizeof(char));
        if (temp_file_names[i] == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for temp_file_names[%d]", i);
            for (int j = 0; j < i; j++) {
                free(temp_file_names[j]);
            }
            free(temp_file_names);
            return;
        }
    }

    // 获取各个目录的文件名列表
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (6/12)");
    bluetooth_send_at_command("AT+M402", CMD_GET_DIR_FILE_NAMES);    // music
    xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_NAMES, pdTRUE, pdFALSE, portMAX_DELAY);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (7/12)");
    bluetooth_send_at_command("AT+M401", CMD_GET_DIR_FILE_NAMES);    // bath
    xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_NAMES, pdTRUE, pdFALSE, portMAX_DELAY);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (8/12)");
    bluetooth_send_at_command("AT+M404", CMD_GET_DIR_FILE_NAMES);    // theme
    xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_NAMES, pdTRUE, pdFALSE, portMAX_DELAY);

    // 储存文件名们到nvs里
    for (int i = 0; i < (music_files_count + bath_files_count + 4); i++) {
        char key[16];
        snprintf(key, sizeof(key), "file_%u", (unsigned int)i);

        err = nvs_set_str(nvs_handle, key, temp_file_names[i]);
        if (err != ESP_OK) {
            ESP_LOGE("get_all_file_names", "Failed to write string to NVS");
            nvs_close(nvs_handle);
            return;
        }
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("get_all_file_names", "Failed to commit changes to NVS");
    }
    
    nvs_close(nvs_handle);

    // 不回到空闲模式, 因为接下来要同步歌的总时长
    // bluetooth_send_at_command("AT+CM0", CMD_CHANGE_TO_IDLE);
    // xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_IDLE, pdTRUE, pdFALSE, portMAX_DELAY);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (9/12)");
    // 宣布初始化完毕
    printf("\n GET FILE NAME LIST END\n");
    xEventGroupSetBits(bt_event_group, EVENT_FILE_LIST_COMPLETE);
}
// 获得音乐库中所有歌的总时长并储存到nvs中
void get_all_music_duration(void) {
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (10/12)");
    bluetooth_send_at_command("AT+M602", CMD_CHANGE_DIR);
    xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_DIR, pdTRUE, pdFALSE, portMAX_DELAY);

    // 获得durations
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (11/12)");
    uint32_t tmp_music_durations[music_files_count];
    for (int i = 0; i < music_files_count; i++) {
        bluetooth_send_at_command("AT+MT", CMD_GET_DURATION);
        xEventGroupWaitBits(music_event_group, EVENT_GET_DURATION, pdTRUE, pdFALSE, portMAX_DELAY);
        tmp_music_durations[i] = current_music_duration;
        bluetooth_send_at_command("AT+CC", CMD_NEXT_TRACK);
        xEventGroupWaitBits(music_event_group, EVENT_NEXT_TRACK, pdTRUE, pdFALSE, portMAX_DELAY);
    }

    // 储存durations到nvs里
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (12/12)");
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("music_durations", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("get_all_music_duration", "Failed to open NVS");
        return;
    }
    for (int i = 0; i < music_files_count; i++) {
        char key[16];
        snprintf(key, sizeof(key), "file_%u", (unsigned int)i);
        err = nvs_set_u32(nvs_handle, key, tmp_music_durations[i]);
        if (err != ESP_OK) {
            ESP_LOGE("get_all_music_duration", "Failed to write duration to NVS");
            nvs_close(nvs_handle);
            return;
        }
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("get_all_file_names", "Failed to commit changes to NVS");
    }
    
    nvs_close(nvs_handle);
    bluetooth_send_at_command("AT+CM0", CMD_CHANGE_TO_IDLE);
    xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_IDLE, pdTRUE, pdFALSE, portMAX_DELAY);
    printf("GET ALL MUSIC DURATION END\n");
    xEventGroupSetBits(bt_event_group, EVENT_ALL_DURATION_COMPLETE);
}
esp_err_t bluetooth_wait_for_response(char *response, size_t max_len) {
    uart_event_t event;
    memset(response, 0, max_len); // 初始化响应缓冲区

    while (1) {
        // 等待UART事件
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    int len = uart_read_bytes(UART_PORT_NUM, response, event.size, portTICK_PERIOD_MS);
                    if (len > 0) {
                        // printf("UART_DATA.length: %d, response: ", len);
                        // for (int i = 0; i < len; i++) {
                        //     // 打印每个字符，对于不可打印字符打印一个点
                        //     char c = response[i];
                        //     if (c >= 32 && c <= 126) {  // 可打印ASCII范围
                        //         putchar(c);
                        //     } else {
                        //         putchar('.');  // 对于控制字符等，打印点
                        //     }
                        // }
                        // printf("\n");
                        // ESP_LOGI(TAG, "Received full response: %s", response);
                        return ESP_OK; // 成功接收到完整响应
                    } else {
                        // 如果读取超时或未读到数据
                        ESP_LOGE(TAG, "读取超时或未读到数据");
                    }
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "UART FIFO Overflow");
                    uart_flush_input(UART_PORT_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "UART Buffer Full");
                    uart_flush_input(UART_PORT_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_BREAK:
                    ESP_LOGW(TAG, "UART Break");
                    return ESP_FAIL;
                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "UART Parity Error");
                    return ESP_FAIL;
                case UART_FRAME_ERR:
                    ESP_LOGW(TAG, "UART Frame Error");
                    return ESP_FAIL;
                default:
                    ESP_LOGW(TAG, "Unhandled UART event type: %d", event.type);
                    return ESP_FAIL;
            }
        }
    }
}

void bluetooth_monitor_task(void *pvParameters) {
    char response[BUF_SIZE];
    while (1) {
        // 等待并接收响应
        if (bluetooth_wait_for_response(response, sizeof(response)) == ESP_OK) {
            // ESP_LOGI(TAG, "命令: %d, 接收到: %s\n", current_command, response);
            printf("命令: %d, 接收到: %s\n", current_command, response);    // 换个颜色
            // 只有响应OK才需要判断是给谁的OK
            if (strncmp(response, "OK", 2) == 0) {
                ESP_LOGI(TAG, "接收到OK: %d", current_command);
                // 暂时没用的就不设置事件组了
                switch (current_command) {
                    case CMD_NEXT_TRACK:
                        xEventGroupSetBits(music_event_group, EVENT_NEXT_TRACK);
                        break;
                    case CMD_PREV_TRACK:
                        xEventGroupSetBits(music_event_group, EVENT_PREV_TRACK);
                        break;
                    case CMD_PLAY_PAUSE_TOGGLE:
                        xEventGroupSetBits(music_event_group, EVENT_PLAY_PAUSE_TOGGLE);
                        break;
                    case CMD_STOP_TRACK:
                        xEventGroupSetBits(music_event_group, EVENT_STOP_TRACK);
                        break;
                    case CMD_PLAY_TRACK:
                        xEventGroupSetBits(music_event_group, EVENT_PLAY_TRACK);
                        break;
                    case CMD_PAUSE_TRACK:
                        xEventGroupSetBits(music_event_group, EVENT_PAUSE_TRACK);
                        break;
                    case CMD_PLAY_MUSIC_WITH_ID:
                        xEventGroupSetBits(music_event_group, EVENT_PLAY_MUSIC_WITH_ID);
                        break;
                    case CMD_SET_VOLUME:
                        xEventGroupSetBits(bt_event_group, EVENT_SET_VOLUME);
                        break;
                    case CMD_BLUETOOTH_SET_NAME:
                        // xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_SET_NAME);
                        // 不可能收到的, 设置完蓝牙名称会复位
                        break;
                    case CMD_BLUETOOTH_SET_PASSWORD:
                        xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_SET_PASSWORD);
                        break;
                    // case CMD_DISCONNECT_BLUETOOTH:
                        // xEventGroupSetBits(bt_event_group, EVENT_DISCONNECT_BLUETOOTH);
                        // break;
                    case CMD_CHANGE_CHANNEL:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_CHANNEL);
                        break;
                    case CMD_CHANGE_TO_BLUETOOTH:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_TO_BLUETOOTH);
                        break;
                    case CMD_CHANGE_TO_MUSIC:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_TO_MUSIC);
                        break;
                    case CMD_CHANGE_TO_IDLE:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_TO_IDLE);
                        break;
                    case CMD_EQUALIZER_SET:
                        xEventGroupSetBits(music_event_group, EVENT_EQUALIZER_SET);
                        break;
                    case CMD_CHANGE_DIR:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_DIR);
                        break;
                    case CMD_CHANGE_PROMPT_TONE:
                        xEventGroupSetBits(bt_event_group, EVENT_CHANGE_PROMPT_TONE);
                        break;
                    default:
                        ESP_LOGE(TAG, "Other CMD: %d", current_command);
                }
            } else if (strncmp(response, "QA+", 3) == 0) {
                ESP_LOGI(TAG, "Volume: %s", response);
                // strncpy(volume_response, response, BUF_SIZE);
                // xEventGroupSetBits(bt_event_group, EVENT_VOLUME_RESPONSE);
            } else if (strncmp(response, "M2+", 3) == 0) {
                sscanf(response, "M2+%d", &current_dir_files_count);
                xEventGroupSetBits(bt_event_group, EVENT_GET_DIR_FILE_COUNT);
            } else if (strncmp(response, "MT+", 3) == 0) {
                sscanf(response, "MT+%lu", &current_music_duration);
                xEventGroupSetBits(music_event_group, EVENT_GET_DURATION);
            } else if (strncmp(response, "btlink", 6) == 0) {
                bluetooth_state = 2;
                xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_CONNECTED);
            } else if (strncmp(response, "btunlink", 8) == 0) {
                bluetooth_state = 1;
                xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_DISCONNECTED);
            } else if (strncmp(response, "END", 3) == 0) {
                xEventGroupSetBits(bt_event_group, EVENT_END_PLAY);
            } else if (strncmp(response, "TD+", 3) == 0) {
                sscanf(response, "TD+%[^\n]", bluetooth_name);
                xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_GET_NAME);
            } else if (strncmp(response, "TE+", 3) == 0) {
                sscanf(response, "TE+%s", bluetooth_password);
                xEventGroupSetBits(bt_event_group, EVENT_BLUETOOTH_GET_PASSWORD);
            } else if (strncmp(response, "MP+", 3) == 0) {
                sscanf(response, "MP+%d", &play_state);
                xEventGroupSetBits(music_event_group, EVENT_GET_PLAY_STATE);
            } else if (strncmp(response, "M4+END", 6) == 0) {
                xEventGroupSetBits(bt_event_group, EVENT_GET_DIR_FILE_NAMES);
            } else if (strncmp(response, "MV+", 3) == 0) {
                sscanf(response, "MV+%d", &device_state);
                xEventGroupSetBits(bt_event_group, EVENT_GET_DEVICE_STATE);
            }
            // 版本号
            else if (strncmp(response, "QV+", 3) == 0) {
                char *bt_ver_start = strstr(response, "QV+V-");
                char *bt_ver_date_start = strstr(response, "VER:");

                if (bt_ver_start && bt_ver_date_start) {
                    bt_ver_start += 5;
                    bt_ver_date_start += 4;

                    char bt_version[10];
                    sscanf(bt_ver_start, "%s", bt_version);
                    
                    char month[4];
                    int day, year;
                    sscanf(bt_ver_date_start, "%s %d %d", month, &day, &year);

                    char final_version[50];
                    char *esp32_version = lv_label_get_text(ui_System_Version_Text);

                    snprintf(final_version, sizeof(final_version), "%s       v%s %s %d %d", esp32_version, bt_version, month, day, year);
                    lv_label_set_text(ui_System_Version_Text, final_version);
                } else {
                    ESP_LOGE(TAG, "版本号错误?");
                }
            } else if (strncmp(response, "QT+", 3) == 0) {
                // 这什么啊, 反正没用
            }
            // QN是提示音相关的查询结果, 不过这里并没有在乎它的正经用法.
            // 上电后主动返回的四条返回值, 这个QN是最后一条, 所以这里拿来当作开机结束的事件
            else if (strncmp(response, "QN+", 3) == 0) {
                xEventGroupSetBits(bt_event_group, EVENT_STARTUP_SUCCESS);
            }
            // 这个是M4响应的一堆文件名的处理
            else if (strncmp(response + 2, "\x5C\x55", 2) == 0) { // 鬼知道这两个字节是什么东西, 拿来当识别头了
                unsigned short utf16_str[BUF_SIZE / 2]; // 用于存储UTF-16编码的字符串
                // 前两个单字节是序号
                utf16_str[0] = response[0];
                utf16_str[1] = response[1];

                int i, j;
                // 丢掉第三四个字节, 处理剩余的字节
                for (i = 2, j = 4; i < (BUF_SIZE - 4) / 2; i++, j+=2) {
                    // 大端转小端
                    utf16_str[i] = response[j] | (response[j + 1] << 8);
                }
                utf16_str[i] = '\0'; // 确保数组末尾有终止符
                // 转为utf8
                char *utf8 = NULL;
                utf16_to_utf8(utf16_str, &utf8);
                printf("UTF8编码的字符串: %s\n", utf8);
                add_file_name(temp_file_names, utf8);
                free(utf8);
            } else {
                // 设置蓝牙名称会复位, 返回个不知道什么东西
                if (current_command == CMD_BLUETOOTH_SET_NAME) {
                    return;
                }
                ESP_LOGE(TAG, "未知返回值, CMD: %d", current_command);
            }
            
        } else {
            ESP_LOGE(TAG, "Failed to get response");
        }
    }
}

