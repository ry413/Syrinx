#ifndef BACKLIGHT_H
#define BACKLIGHT_H


#ifdef __cplusplus
extern "C" {
#endif


#include "stdio.h"
#include "driver/ledc.h"
#include "../lvgl/lvgl.h"

#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_MODE            LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL         LEDC_CHANNEL_0
#define LEDC_PIN             1
#define LEDC_RESOLUTION      LEDC_TIMER_8_BIT

extern lv_obj_t *idle_window;   // 待机界面的指针

// 背光亮度等级与进入待机的时间, 分为7级, 0代表永不进入待机界面
extern uint32_t backlight_level;
extern uint32_t enter_idle_time_level;

void init_backlight(void);
void set_backlight(uint32_t level);
void del_enter_idle_timer(void);
void create_enter_idle_timer(uint32_t timeout_seconds);
void reset_enter_idle_timer(void);
void set_enter_idle_time_to_label(lv_obj_t *target_label, int enter_idle_time_level);
void set_backlight(uint32_t level);

uint32_t enter_idle_time_level_to_second(int level);
int enter_idle_time_second_to_level(uint32_t second);
// uint32_t get_backlight_time_level_from_label(lv_obj_t * target_label);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* BACKLIGHT_H */
