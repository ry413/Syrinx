#include "backlight.h"
#include <esp_log.h>
#include "nvs.h"
#include "stdio.h"
#include "driver/gpio.h"


// 背光定时器是为了进入待机界面, 并不是黑屏
// 那为什么不叫待机定时器?
// 要改得把这个组件都改掉, 不管它了

// 终于把这个改名了, 但是大概就只放在这了吧

uint32_t backlight_level = 3;
uint32_t enter_idle_time_level = 4;

static lv_timer_t *enter_idle_timer;    // 待机定时器
lv_obj_t *idle_window = NULL;           // 待机界面的指针

uint32_t enter_idle_times[7] = {0, 5, 10, 20, 30, 60, 300};   // 待机时间各级表示的秒数, 0代表off 永不进入待机

static uint32_t backlight_level_to_duty(uint32_t level);

// 转换背光亮度等级到指定duty值, 0是熄屏
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
            ESP_LOGE("backlightLevelToDuty", "错误的亮度级别: %ld", level);
            return 255;
    }
}
// 端侧的设置背光的终极函数
void set_backlight(uint32_t level) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, backlight_level_to_duty(level));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
// 待机定时器回调, 进入待机界面
static void enter_idle_callback(lv_timer_t * timer) {
    ESP_LOGI("enter_idle_callback", "进入待机界面");
    lv_scr_load(idle_window);
}

// 初始化背光控制（使用GPIO电平控制）
// void init_backlight(void) {
//     gpio_set_direction(LEDC_PIN, GPIO_MODE_OUTPUT);
//     gpio_set_level(LEDC_PIN, 1);
// }

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
        backlightLevel = 3;
    }
    nvs_close(nvs_handle);
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 20000,
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
// 创建待机定时器, 修改背光时间时也会直接调用这个函数, 也就是说会创建新的定时器
void create_enter_idle_timer(uint32_t timeout_seconds) {
    // 0表示永不进入待机界面
    if(timeout_seconds == 0) {
        return;
    }
    if (enter_idle_timer != NULL) {
        lv_timer_del(enter_idle_timer);
        enter_idle_timer = NULL;
    }
    enter_idle_timer = lv_timer_create(enter_idle_callback, timeout_seconds * 1000, NULL);
    printf("已创建待机定时器\n");
}
// 重置待机定时器, 仅给main.c中的物理触摸回调使用
void reset_enter_idle_timer(void) {
    if (enter_idle_timer != NULL) {
        lv_timer_reset(enter_idle_timer);
        lv_timer_resume(enter_idle_timer);
    }
}
// 删除待机定时器
void del_enter_idle_timer(void) {
    if (enter_idle_timer != NULL) {
        lv_timer_del(enter_idle_timer);
        enter_idle_timer = NULL;
    }
    printf("已删除待机定时器\n");
}
// 更新待机时间到label上
void set_enter_idle_time_to_label(lv_obj_t *target_label, int enter_idle_time_level) {
    uint32_t enter_idle_time = enter_idle_time_level_to_second(enter_idle_time_level);
    // 设置显示的文字
    if (enter_idle_time == 0) {
        lv_label_set_text(target_label, "off");
        return;
    } else {
        lv_label_set_text_fmt(target_label, "%lds", enter_idle_time);
    }
}

// 将传入的待机时间等级转为对应的秒数
uint32_t enter_idle_time_level_to_second(int level) {
    return enter_idle_times[level];
}
// 将传入的待机时间秒数转为对应的等级
int enter_idle_time_second_to_level(uint32_t second) {
    for (int i = 0; i < 7; i++)
        if (enter_idle_times[i] == second)
            return i;
    return 0;
}
