#include "timesync.h"
#include <freertos/event_groups.h>
#include "../lvgl/lvgl.h"
#include "esp_log.h"
#include "../../ui/ui.h"
#include "esp_netif_sntp.h"

// 时间戳
time_t global_time = 0;

// 日期与时间
uint32_t time_hour = 11;
uint32_t time_min = 42;
uint32_t date_year = 2022;
uint32_t date_month = 4;
uint32_t date_day = 13;

lv_obj_t *time_label;
lv_obj_t *date_label;

TaskHandle_t update_time_task_handle = NULL;

bool is_night = false;

void set_time_label(lv_obj_t * label) {
    time_label = label;
}
void set_date_label(lv_obj_t * label) {
    date_label = label;
}
// 获取中文星期几名称
static const char* get_chinese_weekday(int wday) {
    static const char* weekdays[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    return weekdays[wday];
}
// 把global_time更新到time_label上
void update_current_time_label(bool showSymbol) {
    struct tm timeinfo;
    char timeStr[9];

    localtime_r(&global_time, &timeinfo);
    if (showSymbol) {
        strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    } else {
        strftime(timeStr, sizeof(timeStr), "%H %M", &timeinfo);
    }
    if (time_label != NULL) {
        lv_label_set_text(time_label, timeStr);
    } else {
        ESP_LOGE("update_current_time_label", "time_label is NULL");
        set_time_label(ui_Header_Main_Time);
    }
}
// 更新日期到date_label上
void update_current_date_label(void) {
    struct tm timeinfo;
    char date_str[50];

    localtime_r(&global_time, &timeinfo);
    // 手动格式化日期字符串
    snprintf(date_str, sizeof(date_str), "%s/%02d月%02d日", get_chinese_weekday(timeinfo.tm_wday), timeinfo.tm_mon + 1, timeinfo.tm_mday);
    lv_label_set_text(date_label, date_str);
}

// 时间更新定时器
void update_time_task(void *pvParameter)
{
    while (1)
    {
        static bool toggle = true;
        if (time_label == ui_Idle_Window_Time) {
            if (toggle) {
                lv_obj_add_flag(ui_Idle_Window_Time_Decora, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_clear_flag(ui_Idle_Window_Time_Decora, LV_OBJ_FLAG_HIDDEN);
            }
            update_current_time_label(false);
            toggle = !toggle;
        } else {
            update_current_time_label(true);   // 更新当前screen上的时间label
        }
        update_current_date_label();   // 更新待机界面的日期label
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        global_time += 1;

        static int count = 0;
        // printf("count: %d\n", count);
        if (count >= 10) {
            // printf("count >= 10\n");
            uint16_t begin = lv_roller_get_selected(ui_Idle_Time_Settings_Begin); // 获取开始时间
            uint16_t end = lv_roller_get_selected(ui_Idle_Time_Settings_End);     // 获取结束时间
            int now = localtime(&global_time)->tm_hour;                           // 获取当前小时

            // 如果现在处于设定的时间之间, 还在待机界面, 就进入熄屏
            if ((begin <= end && now >= begin && now < end) ||                   // 情况1: 时间不跨午夜
                (begin > end && (now >= begin || now < end))) {                  // 情况2: 时间跨午夜
                // printf("1\n");
                if (!is_night && lv_scr_act() == ui_Idle_Window) {
                    // printf("2\n");
                    is_night = true;
                    offScreen(NULL);
                }
            }
            // 否则, 不处于设定的时间之内, 并处于自动熄屏, 就醒来
            else if (is_night) {
                // printf("不处于设定时间内, 醒来\n");
                onScreen(NULL);
            }
            count = 0;
        }
        count++;
    }
}

// 从NTP获取时间
void obtain_time(void) {

    ESP_LOGI("obtain_time", "Initializing and starting SNTP");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 30;
    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        ESP_LOGI("obtain_time", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    time(&now);
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    mktime(&timeinfo);
    // 保存全局时间
    global_time = mktime(&timeinfo);
    // 启动定时器
    if (update_time_task_handle == NULL) {
        xTaskCreate(update_time_task, "updateTimeTask", 2048, NULL, 5, &update_time_task_handle);
    }
}

time_t calculate_timer_length(int hour, int min) {
    struct tm *current_time_tm = localtime(&global_time);

    // 构造闹钟的时间
    struct tm alarm_time_tm = *current_time_tm;
    alarm_time_tm.tm_hour = hour;
    alarm_time_tm.tm_min = min;
    alarm_time_tm.tm_sec = 0;

    time_t alarm_time = mktime(&alarm_time_tm);

    // 如果设定的时间已经过去，设定到第二天
    if (alarm_time <= global_time) {
        alarm_time += 24 * 3600; // 加一天的秒数
    }

    // 计算时间差
    time_t timer_length = alarm_time - global_time;

    return timer_length;
}


