#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include "driver/ledc.h"

#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_MODE            LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL         LEDC_CHANNEL_0
#define LEDC_PIN             1
#define LEDC_RESOLUTION      LEDC_TIMER_8_BIT

void initBacklight(void);
void setBacklight(uint32_t level);

#endif /* BACKLIGHT_H */
