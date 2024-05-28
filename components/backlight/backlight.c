#include "backlight.h"
#include <esp_log.h>
#include "nvs.h"


// 背光定时器是为了进入待机界面, 并不是黑屏
// 背光定时器是为了进入待机界面, 并不是黑屏


static lv_timer_t *backlight_timer;      // 背光定时器
lv_obj_t *idle_window = NULL;   // 待机界面的指针

uint32_t backlight_times[7] = {0, 5, 10, 20, 30, 60, 300};   // 背光时间各级表示的秒数, 0代表off 永不进入待机



static uint32_t backlight_level_to_duty(uint32_t level);


// 转换背光等级到指定duty值, 0是熄屏
static uint32_t backlight_level_to_duty(uint32_t level) {
    switch (level) {
        case 0:
            return 0;
        case 1:
            return 50;
        case 2:
            return 101;
        case 3:
            return 152;
        case 4:
            return 203;
        case 5:
            return 255;
        default:
            ESP_LOGE("backlightLevelToDuty", "这不应该发生, 错误的亮度级别: %ld", level);
            return 255;
    }
}
// 端侧的设置背光的终极函数
void set_backlight(uint32_t level) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, backlight_level_to_duty(level));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
// 背光定时器回调, 进入待机界面
static void backlight_timer_callback(lv_timer_t * timer) {
    ESP_LOGI("backlight_timer_cb", "Backlight timeout reached, turning off backlight");
    lv_scr_load(idle_window);
    lv_timer_pause(backlight_timer);
}


// 初始化背光, 硬件层面的
void init_backlight(void) {
    // 从nvs获取背光等级
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("BLSettings", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("initBacklight", "Failed to open NVS");
        return;
    }
    uint32_t backlightLevel;
    err = nvs_get_u32(nvs_handle, "level", &backlightLevel);
    if (err != ESP_OK) {
        ESP_LOGE("initBacklight", "Failed to get backlightLevel from NVS");
        nvs_close(nvs_handle);
        return;
    }
    nvs_close(nvs_handle);

    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_USE_APB_CLK
    };
    ledc_channel_config_t ledc_conf = {
        .gpio_num = LEDC_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = backlight_level_to_duty(backlightLevel),
        .hpoint = 0
    };
    ledc_timer_config(&timer_conf);
    ledc_channel_config(&ledc_conf);
}
// 初始化背光定时器, 修改背光时间时也调用这里, 也就是创建新的定时器
void init_backlight_timer(uint32_t timeout_seconds) {
    if (backlight_timer != NULL) {
        lv_timer_del(backlight_timer);
    }
    backlight_timer = lv_timer_create(backlight_timer_callback, timeout_seconds * 1000, NULL);
}
// 重置背光定时器
void reset_backlight_timer(void) {
    if (backlight_timer != NULL) {
        lv_timer_reset(backlight_timer);
    }
}
// 关闭背光定时器
void stop_backlight_timer(void) {
    if (backlight_timer != NULL) {
        lv_timer_del(backlight_timer);
        backlight_timer = NULL;
    }
}
// 更新背光时间到label上
void set_backlight_time_to_label(lv_obj_t *target_label, int backlight_time_level) {
    uint32_t backlight_time = backlight_time_level_to_second(backlight_time_level);
    // 设置显示的文字
    if (backlight_time == 0) {
        lv_label_set_text(target_label, "off");
        return;
    } else {
        lv_label_set_text_fmt(target_label, "%lds", backlight_time);
    }
}

// 将传入的背光时间等级转为对应的秒数
uint32_t backlight_time_level_to_second(int level) {
    return backlight_times[level];
}

int backlight_time_second_to_level(uint32_t second) {
    for (int i = 0; i < 7; i++)
        if (backlight_times[i] == second)
            return i;
    return 0;
}

// // 将传入的label上的秒数转为对应的等级
// uint32_t get_backlight_time_level_from_label(lv_obj_t * target_label) {
//     const char *text = lv_label_get_text(target_label);
//     if (strcmp(text, "off") != 0) {
//         uint32_t backlight_time = atoi(text);
//         for (int i = 0; i < 7; i++) {
//             if (backlight_times[i] == backlight_time) {
//                 return i;
//                 break;
//             }
//         }
//     }
//     return 0;
// }