#ifndef MQTT_H
#define MQTT_H

#ifdef __cplusplus
extern "C" {
#endif


#include "esp_err.h"

#include "mqtt.h"

void mqtt_app_start(void *param);
void mqtt_app_stop(void);
void mqtt_app_cleanup(void);
void mqtt_publish_message(const char *message, int qos, int retain);

#ifdef __cplusplus
}
#endif

#endif // MQTT_H