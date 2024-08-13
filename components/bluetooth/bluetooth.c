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

uint32_t bath_channel_bit = 3;

const char *error_cmd = NULL;
int at_error_code = -1;


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

char final_version[50];

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
    AT_CM(2);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (2/12)");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // 必须进入目录才能保证得到对的M2数值
    AT_M6(2);
    AT_M2();
    music_files_count = current_dir_files_count;

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (3/12)");
    // 再弄个bath目录的文件数
    AT_M6(1);
    AT_M2();
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
    AT_M4(2);
    AT_M4(1);
    AT_M4(4);

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
    // AT_CM(0);

    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (9/12)");
    // 宣布初始化完毕
    printf("\n GET FILE NAME LIST END\n");
    xEventGroupSetBits(bt_event_group, EVENT_FILE_LIST_COMPLETE);
}
// 获得音乐库中所有歌的总时长并储存到nvs中
void get_all_music_duration(void) {
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (10/12)");
    AT_M6(2);

    // 获得durations
    lv_label_set_text(ui_TrackRefreshMsgText, "正在刷新曲目清单中 (11/12)");
    uint32_t tmp_music_durations[music_files_count];
    for (int i = 0; i < music_files_count; i++) {
        AT_MT();
        tmp_music_durations[i] = current_music_duration;
        AT_CC();
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
    AT_CM(0);
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

                    char *esp32_version = "v0.5.4-Vulcan";

                    snprintf(final_version, sizeof(final_version), "%s       v%s %s %d %d", esp32_version, bt_version, month, day, year);
                } else {
                    ESP_LOGE(TAG, "版本号错误?");
                }
                xEventGroupSetBits(bt_event_group, EVENT_QUERY_VERSION);
            } else if (strncmp(response, "QN+", 3) == 0) {
                // 提示音状态, 谁会用啊
            }
            // 上电后主动返回的四条返回值有个QT, 是波特率, 拿来当作复位后的通知
            else if (strncmp(response, "QT+", 3) == 0) {
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
            } else if (strncmp(response, "ERR", 3) == 0) {
                sscanf(response, "ERR%d", &at_error_code);
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

// 打开浴室通道
void open_bath_channel(void) {
    AT_CL(bath_channel_bit);
}
// 打开与浴室通道相反的客厅通道
void open_living_room_channel(void) {
    // if (bath_channel_bit == 2) {
    //     AT_CL(1);
    // } else if (bath_channel_bit == 1){
    //     AT_CL(2);
    // } else {
        AT_CL(3);
    // }
}
// 记录AT错误于主界面的dropdown中
void show_error_info(const char *cmd) {
    lv_obj_clear_flag(ui_Error_Info_Dorpdown, LV_OBJ_FLAG_HIDDEN);
    static int error_info_count = 0;
    if (at_error_code == -1) {
        ESP_LOGE(TAG, "%s Timeout", cmd);
        char error_info[11];
        snprintf(error_info, sizeof(error_info), "%s timeout", cmd);
        lv_dropdown_add_option(ui_Error_Info_Dorpdown, error_info, error_info_count);
        lv_dropdown_set_selected(ui_Error_Info_Dorpdown, error_info_count);
        error_info_count++;
    } else {
        ESP_LOGE(TAG, "%s ERR%d", cmd, at_error_code);
        char error_info[10];
        snprintf(error_info, sizeof(error_info), "%s ERR%d", cmd, at_error_code);
        lv_dropdown_add_option(ui_Error_Info_Dorpdown, error_info, error_info_count);
        lv_dropdown_set_selected(ui_Error_Info_Dorpdown, error_info_count);
        error_info_count++;
        at_error_code = -1;
    }
}

// 查询设备状态(tf卡在不在)
void AT_MV(void) {
    bluetooth_send_at_command("AT+MV", CMD_GET_DEVICE_STATE);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_GET_DEVICE_STATE, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_GET_DEVICE_STATE)) {
        show_error_info("MV");
    }
}

// 切换功放
void AT_CL(int channel) {
    char command[10];
    snprintf(command, sizeof(command), "AT+CL%d", channel);
    bluetooth_send_at_command(command, CMD_CHANGE_CHANNEL);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_CHANNEL, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_CHANGE_CHANNEL)) {
        char str[4];
        snprintf(str, sizeof(str), "CL%d", channel);
        show_error_info(str);
    }
}

// 切换工作模式
void AT_CM(int mode) {
    EventBits_t bits = 0;

    if (mode == 0) {
        bluetooth_send_at_command("AT+CM0", CMD_CHANGE_TO_IDLE);
        bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_IDLE, pdTRUE, pdFALSE, 3000 / portTICK_PERIOD_MS);
    } else if (mode == 1) {
        bluetooth_send_at_command("AT+CM1", CMD_CHANGE_TO_BLUETOOTH);
        bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_BLUETOOTH, pdTRUE, pdFALSE, 3000 / portTICK_PERIOD_MS);
    } else if (mode == 2) {
        bluetooth_send_at_command("AT+CM2", CMD_CHANGE_TO_MUSIC);
        bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_TO_MUSIC, pdTRUE, pdFALSE, 3000 / portTICK_PERIOD_MS);
    } else {
        ESP_LOGE(TAG, "Error mode: %d", mode);
        return;
    }
    
    if (bits != 0) {
        work_mode = mode;
    } else {
        char str[4];
        snprintf(str, sizeof(str), "CM%d", mode);
        show_error_info(str);
    }
}
// 获得当前目录下的曲目数
void AT_M2(void) {
    bluetooth_send_at_command("AT+M2", CMD_GET_DIR_FILE_COUNT);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_COUNT, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_GET_DIR_FILE_COUNT)) {
        show_error_info("M2");
    }
}
// 进入指定目录
void AT_M6(int dir_id) {
    char command[10];
    snprintf(command, sizeof(command), "AT+M6%02d", dir_id);
    bluetooth_send_at_command(command, CMD_CHANGE_DIR);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_DIR, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_CHANGE_DIR)) {
        char str[5];
        snprintf(str, sizeof(str), "M6%02d", dir_id);
        show_error_info(str);
    }
}
// 开始获得当前目录下所有曲目名
void AT_M4(int m4_target) {
    char command[10];
    snprintf(command, sizeof(command), "AT+M4%02d", m4_target);
    bluetooth_send_at_command(command, CMD_GET_DIR_FILE_NAMES);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_GET_DIR_FILE_NAMES, pdTRUE, pdFALSE, portMAX_DELAY);
    if (!(bits & EVENT_GET_DIR_FILE_NAMES)) {
        char str[5];
        snprintf(str, sizeof(str), "M4%02d", m4_target);
        show_error_info(str);
    }
}
// 获得当前曲目的总时长
void AT_MT(void) {
    bluetooth_send_at_command("AT+MT", CMD_GET_DURATION);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_GET_DURATION, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_GET_DURATION)) {
        show_error_info("MT");
    }
}
// 下一首
void AT_CC(void) {
    bluetooth_send_at_command("AT+CC", CMD_NEXT_TRACK);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_NEXT_TRACK, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_NEXT_TRACK)) {
        show_error_info("CC");
    }
}
// 播放指定物理序号的音乐
void AT_AF(int music_id) {
    char command[10];
    snprintf(command, sizeof(command), "AT+AF%02d", music_id);
    bluetooth_send_at_command(command, CMD_PLAY_MUSIC_WITH_ID);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_PLAY_MUSIC_WITH_ID, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_PLAY_MUSIC_WITH_ID)) {
        char str[5];
        snprintf(str, sizeof(str), "AF%02d", music_id);
        show_error_info(str);
    }
}
// 修改提示音开关
void AT_CN(int cmd) {       // 这b参数要怎么命名啊
    char command[10];
    snprintf(command, sizeof(command), "AT+CN%d", cmd);
    bluetooth_send_at_command(command, CMD_CHANGE_PROMPT_TONE);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_CHANGE_PROMPT_TONE, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_CHANGE_PROMPT_TONE)) {
        char str[4];
        snprintf(str, sizeof(str), "CN%d", cmd);
        show_error_info(str);
    }
}
// 停止播放
void AT_AA0(void) {
    bluetooth_send_at_command("AT+AA0", CMD_STOP_TRACK);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_STOP_TRACK, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_STOP_TRACK)) {
        show_error_info("AA0");
    }
}
// 获得蓝牙名称
void AT_TD(void) {
    bluetooth_send_at_command("AT+TD", CMD_BLUETOOTH_GET_NAME);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_BLUETOOTH_GET_NAME, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_BLUETOOTH_GET_NAME)) {
        show_error_info("TD");
    }
}
// 获得蓝牙密码
void AT_TE(void) {
    bluetooth_send_at_command("AT+TE", CMD_BLUETOOTH_GET_PASSWORD);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_BLUETOOTH_GET_PASSWORD, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_BLUETOOTH_GET_PASSWORD)) {
        show_error_info("TE");
    }
}
// 设置蓝牙密码
void AT_BE(char *pass) {
    char command[20];
    snprintf(command, sizeof(command), "AT+BE%s", pass);
    bluetooth_send_at_command(command, CMD_BLUETOOTH_SET_PASSWORD);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_BLUETOOTH_SET_PASSWORD, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_BLUETOOTH_SET_PASSWORD)) {
        show_error_info("BE");
    }
}
// 设置蓝牙名称
void AT_BD(char *name) {
    char command[20];
    snprintf(command, sizeof(command), "AT+BD%s", name);
    bluetooth_send_at_command(command, CMD_BLUETOOTH_SET_NAME);
    // 然后会自动复位, 所以等待复位事件位
    vTaskDelay(100 / portTICK_PERIOD_MS);   // QT是第二个回复的, 而复位会回复四个值, 所以等一下
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_STARTUP_SUCCESS, pdTRUE, pdFALSE, 3000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_STARTUP_SUCCESS)) {
        show_error_info("BD");
    }
}
// 设置音量
void AT_CA(int volume) {
    char command[20];
    snprintf(command, sizeof(command), "AT+CA%02d", volume);
    bluetooth_send_at_command(command, CMD_SET_VOLUME);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_SET_VOLUME, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_SET_VOLUME)) {
        char str[5];
        snprintf(str, sizeof(str), "CA%02d", volume);
        show_error_info(str);
    }
}
// 设置均衡器
void AT_CQ(int equalizer) {
    char command[20];
    snprintf(command, sizeof(command), "AT+CQ%d", equalizer);
    bluetooth_send_at_command(command, CMD_EQUALIZER_SET);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_EQUALIZER_SET, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_EQUALIZER_SET)) {
        char str[4];
        snprintf(str, sizeof(str), "CQ%d", equalizer);
        show_error_info(str);
    }
}
// 播放/暂停
void AT_CB(void) {
    bluetooth_send_at_command("AT+CB", CMD_PLAY_PAUSE_TOGGLE);
    EventBits_t bits = xEventGroupWaitBits(music_event_group, EVENT_PLAY_PAUSE_TOGGLE, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_PLAY_PAUSE_TOGGLE)) {
        show_error_info("CB");
    }
}
// 查询版本号
void AT_QV(void) {
    bluetooth_send_at_command("AT+QV", CMD_QUERY_VERSION);
    EventBits_t bits = xEventGroupWaitBits(bt_event_group, EVENT_QUERY_VERSION, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
    if (!(bits & EVENT_QUERY_VERSION)) {
        show_error_info("QV");
    }
}




