#include "rs485.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "string.h"

#include "../backlight/backlight.h"
#include "../bluetooth/bluetooth.h"
#include "../timesync/timesync.h"
#include "../../main/touch.h"

#include "../../ui/ui_events.h"

static const char *TAG = "rs485";

typedef struct {
    uint8_t header;
    uint8_t command[5];
    uint8_t checksum;
    uint8_t footer;
} rs485_packet_t;

esp_err_t rs485_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    ESP_ERROR_CHECK(uart_param_config(RS485_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(RS485_UART_PORT, RS485_TX_GPIO_NUM, RS485_RX_GPIO_NUM, RS485_DE_GPIO_NUM, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(RS485_UART_PORT, 1024 * 2, 0, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_set_mode(RS485_UART_PORT, UART_MODE_RS485_HALF_DUPLEX));

    ESP_LOGI(TAG, "UART 1 初始化成功");
    return ESP_OK;
}
// 检查校验和
uint8_t calculate_checksum(rs485_packet_t *packet) {
    uint8_t checksum = 0;
    checksum += packet->header;
    for (size_t i = 0; i < sizeof(packet->command); i++) {
        checksum += packet->command[i];
    }
    return checksum & 0xFF;
}
// 判断指令类型并处理
void process_command(rs485_packet_t *packet, size_t len) {
    // 确保包长度正确
    if (len != PACKET_SIZE) {
        ESP_LOGE(TAG, "Invalid packet length: %d", len);
        return;
    }

    // 检查包头和包尾
    if (packet->header != PACKET_HEADER || packet->footer != PACKET_FOOTER) {
        ESP_LOGE(TAG, "Invalid packet header/footer");
        return;
    }

    // 检查校验和
    uint8_t calculated_chk = calculate_checksum(packet);
    if (calculated_chk != packet->checksum) {
        ESP_LOGE(TAG, "Checksum error: calculated 0x%02X, received 0x%02X", calculated_chk, packet->checksum);
        return;
    }

    // ******************** 指令 ********************
    EventGroupHandle_t event_group = get_bluetooth_event_group();
    EventBits_t bits;

    // 插卡指令
    if (memcmp(packet->command, (uint8_t[]){0x80, 0x01, 0x00, 0x26, 0x01}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 插卡");
        set_backlight(backlight_level);                                  // 1.打开背光
        enable_touch();                                                     // 2.启用触摸
    }
    // 拔卡指令
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x01, 0x00, 0x26, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 拔卡");
        set_backlight(0);                                                   // 1.关闭背光
        disabled_touch();                                                   // 2.禁用触摸
        if (is_music_mode) {
            bluetooth_send_at_command("AT+AA0", CMD_STOP_STATE);            // 3.停止播放
            bits = xEventGroupWaitBits(event_group, EVENT_STOP_STATE, pdTRUE, pdFALSE, portMAX_DELAY);
            if (bits & EVENT_STOP_STATE) {
                bluetooth_send_at_command("AT+CU1", CMD_ON_MUTE);           // 4.静音
            }
        } else {
            bluetooth_send_at_command("AT+BA1", CMD_DISCONNECT_BLUETOOTH);  // 3.断开蓝牙
            bits = xEventGroupWaitBits(event_group, EVENT_DISCONNECT_BLUETOOTH, pdTRUE, pdFALSE, portMAX_DELAY);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (bits & EVENT_DISCONNECT_BLUETOOTH) {
                bluetooth_send_at_command("AT+CU1", CMD_ON_MUTE);           // 4.静音
            }
        }
    }
    // 睡眠模式
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x1E, 0x01, 0x0A, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 睡眠模式");
        offScreen(NULL);
    }
    // 明亮模式
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x1E, 0x01, 0x01, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 明亮模式");
        onScreen(NULL);
    }
    // 播放
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x02}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 播放");
        bluetooth_send_at_command("AT+AA1", CMD_PLAY_STATE);
    }
    // 停止
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x03}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 停止");
        bluetooth_send_at_command("AT+AA0", CMD_STOP_STATE);
    }
    // 上一首
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x04}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 上一首");
        prevTrack(NULL);
    }
    // 下一首
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x05}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 下一首");
        nextTrack(NULL);
    }
    // 暂停/播放
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x0C}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 暂停/播放");
        playPause(NULL);
    }
    // 时间同步
    else if (packet->command[0] == 0x78) {
        uint32_t timestamp = (packet->command[1] << 24) | (packet->command[2] << 16) |
                         (packet->command[3] << 8) | packet->command[4];
        // 使用时间戳进行后续处理
        printf("Received timestamp: %lu\n", timestamp);
        global_time = timestamp + 8 * 3600;

        // 启动定时器
        if (update_time_task_handle == NULL) {
            xTaskCreate(update_time_task, "updateTimeTask", 2048, NULL, 5, &update_time_task_handle);
        }

    }
    // 浴室播放
    else if (memcmp(packet->command, (uint8_t[]){0x16, 0xA0, 0x00, 0x02, 0x00}, (size_t)5) == 0) {

    }
    // 浴室停止播放
    else if (memcmp(packet->command, (uint8_t[]){0x78, 0x60, 0xE3, 0x26, 0x02}, (size_t)5) == 0) {

    }
    // 未知指令
    else {
        // ESP_LOGE(TAG, "Unknown command");
    }
}
void rs485_monitor_task(void *pvParameter) {
    rs485_packet_t packet;
    while (1) {
        int len = uart_read_bytes(RS485_UART_PORT, (uint8_t*)&packet, PACKET_SIZE, portMAX_DELAY);
        
        if (len > 0) {
            process_command(&packet, len);
        } else if (len < 0) {
            ESP_LOGE(TAG, "UART read error: %d", len);
        }
    }
}
