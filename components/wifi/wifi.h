#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
extern "C" {
#endif


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

struct wifi_config_params {
    char ssid[32];
    char password[64];
};

extern bool wifi_is_connected;

extern uint8_t wifi_enabled;
extern char *wifi_ssid;
extern char *wifi_password;

extern EventGroupHandle_t wifi_event_group;

void wifi_init(void);
void start_wifi_connect_task(const char* ssid, const char* password);
void wifi_disconnect(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif // WIFI_H
