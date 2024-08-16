#ifndef RS485_H
#define RS485_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "esp_err.h"

#include "../backlight/backlight.h"
#include "../bluetooth/bluetooth.h"
#include "../timesync/timesync.h"
#include "../../main/touch.h"



// RS485 配置参数
#define RS485_UART_PORT      UART_NUM_1
#define RS485_BAUD_RATE      9600
#define RS485_TX_GPIO_NUM    6
#define RS485_RX_GPIO_NUM    4
#define RS485_DE_GPIO_NUM    5

#define PACKET_SIZE          8
// 协议头和尾
#define PACKET_HEADER         0x7F
#define PACKET_FOOTER         0x7E


// 初始化 RS485 接口
esp_err_t rs485_init(void);
void rs485_monitor_task(void *pvParameter);

extern TaskHandle_t bath_play_task_handle;
extern TaskHandle_t music_play_task_handle;    // 这些实际上不应该放这, 但是放ui_events.h里会导致idf爆炸, 有生之年应该要解耦一下
extern TaskHandle_t nature_play_task_handle;
extern TaskHandle_t alarm_clock_cycle_shout_task_handle;

void show_bath_sound_icon_callback(void *param);
void hide_bath_sound_icon_callback(void *param);

#ifdef __cplusplus
}
#endif

#endif // RS485_H