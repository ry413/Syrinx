#include "wifi.h"
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define EXAMPLE_ESP_WIFI_SSID      "xz526"
#define EXAMPLE_ESP_WIFI_PASS      "xz526888"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

static const char *TAG = "wifi_station";
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void) {
    
    // ESP_LOGI("WIFI INIT1: ", "Free internal memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // ESP_LOGI("WIFI INIT1: ", "Free DMA memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    // ESP_LOGI("WIFI INIT1: ", "Free SPIRAM memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.static_rx_buf_num = 10;  // 设置静态接收缓冲区数量
    cfg.dynamic_rx_buf_num = 32; // 设置动态接收缓冲区数量
    cfg.static_tx_buf_num = 10;  // 设置静态发送缓冲区数量
    cfg.dynamic_tx_buf_num = 32; // 设置动态发送缓冲区数量

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .listen_interval = 10,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    
    // ESP_LOGI("WIFI INIT2: ", "Free internal memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // ESP_LOGI("WIFI INIT2: ", "Free DMA memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    // ESP_LOGI("WIFI INIT2: ", "Free SPIRAM memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));


    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    // ESP_LOGI("WIFI INIT3: ", "Free internal memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // ESP_LOGI("WIFI INIT3: ", "Free DMA memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    // ESP_LOGI("WIFI INIT3: ", "Free SPIRAM memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

}

EventGroupHandle_t get_wifi_event_group(void) {
    return s_wifi_event_group;
}

void wifi_task(void *pvParameter) {
    wifi_init_sta();
    vTaskDelete(NULL);
}

void wifi_disconnect(void) {
    esp_err_t err = esp_wifi_disconnect();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "WiFi disconnected successfully.");
    } else {
        ESP_LOGE(TAG, "WiFi disconnection failed: %s", esp_err_to_name(err));
    }
}