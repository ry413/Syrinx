#include "wifi.h"
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "string.h"
#include "../timesync/timesync.h"
#include "../../ui/ui_events.h"


#define MAX_RECONNECT_ATTEMPTS  5

static const char *TAG = "wifi_station";
EventGroupHandle_t wifi_event_group = NULL;

bool wifi_is_connected = false;
uint8_t wifi_enabled = 0;
char *wifi_name = "12345678";
char *wifi_password = "12345678";
static TaskHandle_t wifi_connect_task_handle = NULL;
static bool should_reconnect = true;        // 断开后是否重连

static int reconnect_attempts = 0;      // 重连次数



static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        reconnect_attempts++;
        if (should_reconnect && reconnect_attempts <= MAX_RECONNECT_ATTEMPTS) {
            ESP_LOGI(TAG, "WiFi连接中: (%d/%d)", reconnect_attempts, MAX_RECONNECT_ATTEMPTS);
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        } else {
            if (reconnect_attempts >= MAX_RECONNECT_ATTEMPTS) {
                ESP_LOGE(TAG, "已达到最大重连次数, 停止连接");
                xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
                should_reconnect = false;
            } else {
                ESP_LOGI(TAG, "WiFi已断开");
            }
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        reconnect_attempts = 0;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 初始化 WiFi 配置
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.static_rx_buf_num = 10;  // 设置静态接收缓冲区数量
    cfg.dynamic_rx_buf_num = 32; // 设置动态接收缓冲区数量
    cfg.static_tx_buf_num = 10;  // 设置静态发送缓冲区数量
    cfg.dynamic_tx_buf_num = 32; // 设置动态发送缓冲区数量

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    wifi_event_group = xEventGroupCreate();
    ESP_LOGI(TAG, "WiFi initialized.");
}
static void wifi_connect_task(void *pvParameter) {
    struct wifi_config_params* params = (struct wifi_config_params*) pvParameter;

    // 检查 WiFi 是否已初始化
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);

    if (err == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGE(TAG, "WiFi is not initialized.");
        free(params);
        vTaskDelete(NULL);
        return;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get WiFi mode: %s", esp_err_to_name(err));
        free(params);
        vTaskDelete(NULL);
        return;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .listen_interval = 10,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", params->ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", params->password);

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    should_reconnect = true;
    reconnect_attempts = 0;
    
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi已连接 SSID:%s", params->ssid);
        wifi_is_connected = true;
        setWifiStateIcon(true);
        obtain_time();
        srand(global_time);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGW(TAG, "WiFi未连接 SSID:%s", params->ssid);
        wifi_is_connected = false;
        setWifiStateIcon(false);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        wifi_is_connected = false;
    }

    free(params);
    wifi_connect_task_handle = NULL;
    vTaskDelete(NULL);
}

void start_wifi_connect_task(const char* ssid, const char* password) {
    if (wifi_is_connected) {
        ESP_LOGI(TAG, "WIFI已连接, 拒绝重复连接");
        return;
    }
    if (wifi_connect_task_handle != NULL) {
        vTaskDelete(wifi_connect_task_handle);
        wifi_connect_task_handle = NULL;
        ESP_LOGW(TAG, "WiFi正在连接, 已重新开始连接");
    }
    struct wifi_config_params* params = malloc(sizeof(struct wifi_config_params));
    if (params == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for wifi_config_params");
        return;
    }

    snprintf(params->ssid, sizeof(params->ssid), "%s", ssid);
    snprintf(params->password, sizeof(params->password), "%s", password);
    assert(wifi_connect_task_handle == NULL);
    xTaskCreate(&wifi_connect_task, "wifi_connect_task", 4096, (void*) params, 5, &wifi_connect_task_handle);
}

void wifi_disconnect(void) {
    if (wifi_is_connected == false) {
        ESP_LOGI(TAG, "WIFI未连接, 拒绝断开");
        return;
    }
    should_reconnect = false;
    esp_err_t err = esp_wifi_disconnect();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "WiFi disconnected successfully.");
        wifi_is_connected = false;
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        if (wifi_connect_task_handle != NULL) {
            vTaskDelete(wifi_connect_task_handle);
            wifi_connect_task_handle = NULL;
        }
    } else {
        ESP_LOGE(TAG, "WiFi disconnection failed: %s", esp_err_to_name(err));
    }
}