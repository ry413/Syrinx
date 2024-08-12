#ifndef TIMESYNC_H
#define TIMESYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>
#include "esp_err.h"
#include <time.h>
#include <esp_sntp.h>
#include "../lvgl/lvgl.h"


extern time_t global_time;

extern uint32_t time_hour;
extern uint32_t time_min;
extern uint32_t date_year;
extern uint32_t date_month;
extern uint32_t date_day;

extern TaskHandle_t update_time_task_handle;

void update_current_time_label(void);
void update_current_date_label(void);
void obtain_time(void);
void set_time_label(lv_obj_t * label);
void set_date_label(lv_obj_t * label);
void update_time_task(void *pvParameter);
time_t calculate_timer_length(int hour, int min);
#ifdef __cplusplus
}
#endif

#endif // TIMESYNC_H