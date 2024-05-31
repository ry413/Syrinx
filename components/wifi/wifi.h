#ifndef WIFI_H
#define WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void init_wifi_task(void *pvParameter);
EventGroupHandle_t get_wifi_event_group(void);
void wifi_disconnect(void);

#endif // WIFI_H
