#include "bluetooth.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <string.h>
// #include "iconv.h"
#include <locale.h>
#include <wchar.h>

#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        GPIO_NUM_43
#define UART_RX_PIN        GPIO_NUM_44
#define BUF_SIZE           1024
#define QUEUE_SIZE         10

static const char *TAG = "Bluetooth";
static QueueHandle_t uart_queue;

// 将UTF-16小端编码转换为UTF-8
char* utf16le_to_utf8(const uint16_t *utf16_str, size_t utf16_len) {
    setlocale(LC_ALL, "en_US.utf8");

    // 计算转换后的UTF-8字符串所需的长度
    size_t utf8_len = wcstombs(NULL, (const wchar_t *)utf16_str, 0) + 1;
    if (utf8_len == (size_t)-1) {
        return NULL;
    }

    // 分配UTF-8字符串的内存
    char *utf8_str = (char *)malloc(utf8_len);
    if (!utf8_str) {
        return NULL;
    }

    // 执行转换
    wcstombs(utf8_str, (const wchar_t *)utf16_str, utf8_len);
    return utf8_str;
}

void process_mixed_encoding(const uint8_t *bytes, size_t len) {
    char final_response[1024] = {0}; // 存储最终的UTF-8字符串
    char *ptr = final_response;
    size_t remaining_len = sizeof(final_response) - 1;

    for (size_t i = 0; i < len; ) {
        if (i + 1 < len && (bytes[i] <= 0x7F) && (bytes[i + 1] & 0x80)) {
            // 检测到可能的UTF-16 LE编码字符（中文字符）
            uint16_t utf16_char = bytes[i] | (bytes[i + 1] << 8);
            i += 2;

            // 将UTF-16 LE字符转换为UTF-8
            uint16_t utf16_str[2] = {utf16_char, 0}; // 确保空终止
            char *utf8_char = utf16le_to_utf8(utf16_str, 1);
            if (utf8_char) {
                strncat(ptr, utf8_char, remaining_len);
                remaining_len -= strlen(utf8_char);
                ptr += strlen(utf8_char);
                free(utf8_char);
            }
        } else {
            // 处理UTF-8字符
            if (remaining_len > 0) {
                *ptr++ = bytes[i++];
                remaining_len--;
            } else {
                break;
            }
        }
    }

    printf("Final UTF-8 response: %s\n", final_response);
}

esp_err_t bluetooth_init(void) {
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
    ESP_LOGI(TAG, "UART 0 初始化成功");
    return ESP_OK;
}

esp_err_t bluetooth_send_at_command(const char *command) {
    char command_with_newline[128];
    snprintf(command_with_newline, sizeof(command_with_newline), "%s\r\n", command);

    int len = uart_write_bytes(UART_PORT_NUM, command_with_newline, strlen(command_with_newline));
    if (len < 0) {
        ESP_LOGE(TAG, "UART 0 write error");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Send AT command: %s", command);
        return ESP_OK;
    }
}
void send_at_command(const char *cmd, char *response, int resp_size) {
    uart_flush(UART_PORT_NUM);
    uart_write_bytes(UART_PORT_NUM, cmd, strlen(cmd));
    uart_wait_tx_done(UART_PORT_NUM, 1000 / portTICK_PERIOD_MS);
    int len = uart_read_bytes(UART_PORT_NUM, response, resp_size - 1, 1000 / portTICK_PERIOD_MS);
    printf("read length: %d", len);
    response[len] = '\0'; // 确保字符串终止
}
void get_all_files() {
    char response[BUF_SIZE];
    int total_files = 0;

    // 获取总文件数量
    send_at_command("AT+M2\r\n", response, BUF_SIZE);
    // sscanf(response, "M2+%d", &total_files); // 修改解析格式为 "M2+数字"
    ESP_LOGI(TAG, "Total files: %s", response);

    // 遍历所有文件
    for (int i = 0; i < total_files; i++) {
        // 获取当前文件名
        send_at_command("AT+MF\r\n", response, BUF_SIZE);
        ESP_LOGI(TAG, "File %d: %s", i + 1, response);

        // 播放下一曲以切换到下一个文件
        if (i < total_files - 1) {
            send_at_command("AT+CC\r\n", response, BUF_SIZE);
        }
    }
}

void at_task(void *pvParameters) {
    get_all_files();
    vTaskDelete(NULL);
}
esp_err_t bluetooth_wait_for_response(char *response, size_t max_len) {
    uart_event_t event;
    while (1) {
        // 等待 UART 事件
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                ESP_LOGI(TAG, "event.type == UART_DATA");
                int total_len = 0;
                int len;
                while (total_len < max_len - 1) {
                    len = uart_read_bytes(UART_PORT_NUM, (uint8_t *)(response + total_len), max_len - 1 - total_len, 1000 / portTICK_PERIOD_MS);
                    if (len > 0) {
                        // 过滤掉空字节并记录有效字节
                        int valid_len = 0;
                        for (int i = 0; i < len; i++) {
                            if (response[total_len + i] != '\0') {
                                response[total_len + valid_len] = response[total_len + i];
                                valid_len++;
                            }
                        }
                        total_len += valid_len;
                        response[total_len] = '\0'; // Null-terminate the response

                        // Log every received byte for debugging
                        for (int i = 0; i < valid_len; i++) {
                            ESP_LOGI(TAG, "Received byte: 0x%02X", response[total_len - valid_len + i]);
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
                    response[total_len] = '\0'; // Null-terminate the response
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
            }
        }
    }
}

void bluetooth_task(void *pvParameters) {
    char response[BUF_SIZE];
    while (1) {
        // 等待并接收响应
        if (bluetooth_wait_for_response(response, sizeof(response)) == ESP_OK) {
            ESP_LOGI(TAG, "Final response: %s", response);
            if (strstr(response, "OK") != NULL) {
                ESP_LOGI(TAG, "Command executed successfully: %s", response);
            } else if (strstr(response, "ERR") != NULL) {
                ESP_LOGE(TAG, "Error response received: %s", response);
            } else {
                ESP_LOGI(TAG, "Response: %s", response);
            }
        } else {
            ESP_LOGE(TAG, "Failed to get response");
        }
    }
}

