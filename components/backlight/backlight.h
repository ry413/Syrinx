#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include "stdio.h"
#include "driver/ledc.h"
#include "../lvgl/lvgl.h"

#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_MODE            LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL         LEDC_CHANNEL_0
#define LEDC_PIN             1
#define LEDC_RESOLUTION      LEDC_TIMER_8_BIT

extern lv_obj_t *idle_window;   // 待机界面的指针

// 背光亮度等级与时间, 背光时间分为7级, 0代表永不熄灭
extern uint32_t backlight_level;
extern uint32_t backlight_time_level;

void init_backlight(void);
void set_backlight(uint32_t level);
void stop_backlight_timer(void);
void init_backlight_timer(uint32_t timeout_seconds);
void reset_backlight_timer(void);
void set_backlight_time_to_label(lv_obj_t *target_label, int backlight_time_level);
void set_backlight(uint32_t level);

uint32_t backlight_time_level_to_second(int level);
int backlight_time_second_to_level(uint32_t second);
// uint32_t get_backlight_time_level_from_label(lv_obj_t * target_label);

#endif /* BACKLIGHT_H */
