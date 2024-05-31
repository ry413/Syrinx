#ifndef RS485_H
#define RS485_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "esp_err.h"

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



#ifdef __cplusplus
}
#endif

#endif // RS485_H