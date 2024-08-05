#include "timesync.h"
#include <freertos/event_groups.h>
#include "../lvgl/lvgl.h"
#include "esp_log.h"

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
void update_current_time_label(void) {
    struct tm timeinfo;
    char timeStr[9];

    localtime_r(&global_time, &timeinfo);
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    lv_label_set_text(time_label, timeStr);
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
        update_current_time_label();   // 更新当前screen上的时间label
        update_current_date_label();   // 更新待机界面的日期label
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        global_time += 1;
    }
}

// 从NTP获取时间
void obtain_time(void) {
    // NTP服务器列表, 这是macOS上使用ntpdate测试出的一些延迟低的服务器, 2024.5.15
    const char* ntp_servers[] = {
        "cn.ntp.org.cn",
        // "edu.ntp.org.cn",
        // "ntp.sjtu.edu.cn",
        // "ntp.aliyun.com",
        // "pool.ntp.org",
    };
    const int num_ntp_servers = sizeof(ntp_servers) / sizeof(ntp_servers[0]);

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 20;
    
    if (esp_sntp_enabled()) {
        // 放个IP
        ip_addr_t *addr = NULL;
        ipaddr_aton("111.230.50.201", addr);
        esp_sntp_setserver(0, addr);

        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        for (int i = 0; i < num_ntp_servers; ++i) {
            esp_sntp_setservername(i, ntp_servers[i]);
        }
        esp_sntp_init();
    }

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI("obtainTime", "等待系统时间设置... (%d/%d) 正在尝试连接服务器: %s", retry, retry_count, ntp_servers[retry % num_ntp_servers]);

        time(&now);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        localtime_r(&now, &timeinfo);
    }

    if (retry >= retry_count) {
        ESP_LOGE("obtainTime", "在 %d 次重试后无法获取时间。请检查您的网络连接。", retry_count);
    } else {
        ESP_LOGI("obtainTime", "系统时间已设置。");
        // 修正时区
        timeinfo.tm_hour += 8;
        mktime(&timeinfo);
        // 保存全局时间
        global_time = mktime(&timeinfo);
        // 启动定时器
        if (update_time_task_handle == NULL) {
            xTaskCreate(update_time_task, "updateTimeTask", 2048, NULL, 5, &update_time_task_handle);
        }
    }
}



