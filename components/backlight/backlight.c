#include "backlight.h"
#include <esp_log.h>
#include "nvs.h"

static uint32_t backlightLevelToDuty(uint32_t level);


void initBacklight(void) {
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
        .duty = backlightLevelToDuty(backlightLevel),
        .hpoint = 0
    };
    ledc_timer_config(&timer_conf);
    ledc_channel_config(&ledc_conf);
}
static uint32_t backlightLevelToDuty(uint32_t level) {
    switch (level) {
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
            return 1;
    }
}
void setBacklight(uint32_t level) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, backlightLevelToDuty(level));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}