#include "mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "esp_mac.h"

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

#include "../bluetooth/bluetooth.h"
#include "../../ui/ui.h"


static const char *TAG = "mqtt";

esp_mqtt_client_handle_t client;

char str_mac[20];
char full_topic[40];

typedef struct {
    bool force_update;               // 是否强制升级
    char cmd_type[7];                // 命令类型
    char ota_url[128];               // 众望所归的OTA URL
    char ota_ver[20];                // OTA 版本
    char device_name[10];            // 设备名称, XZ-BGS3-A
    char latest_version[20];         // 世界上, 此时固件最新的版本号, 暂时没用
} ParsedData_t;

ParsedData_t parsed;

bool parse_data(const char *data) {
    char data_copy[512];
    strncpy(data_copy, data, sizeof(data_copy));
    data_copy[sizeof(data_copy) - 1] = '\0'; // 确保字符串终止

    // 使用strtok按逗号分隔
    char *token = strtok(data_copy, ",");
    if (!token) return false; // 检查是否有第一个字段

    // 解析第一个参数：force_update
    parsed.force_update = (strcmp(token, "true") == 0);

    // 解析第二个参数：cmd_type
    token = strtok(NULL, ",");
    if (!token) return false; // 检查第二个字段
    strncpy(parsed.cmd_type, token, sizeof(parsed.cmd_type) - 1);
    parsed.cmd_type[sizeof(parsed.cmd_type) - 1] = '\0';

    // 解析第三个参数：ota_url
    token = strtok(NULL, ",");
    if (!token) return false; // 检查第三个字段
    strncpy(parsed.ota_url, token, sizeof(parsed.ota_url) - 1);
    parsed.ota_url[sizeof(parsed.ota_url) - 1] = '\0';

    // 解析第四个参数：ota_ver
    token = strtok(NULL, ",");
    if (!token) return false; // 检查第四个字段
    strncpy(parsed.ota_ver, token, sizeof(parsed.ota_ver) - 1);
    parsed.ota_ver[sizeof(parsed.ota_ver) - 1] = '\0';

    // 解析第五个参数：device_name
    token = strtok(NULL, ",");
    if (!token) return false; // 检查第五个字段
    strncpy(parsed.device_name, token, sizeof(parsed.device_name) - 1);
    parsed.device_name[sizeof(parsed.device_name) - 1] = '\0';

    // 解析第六个参数：latest_version
    token = strtok(NULL, ",");
    if (!token) return false; // 检查第六个字段
    strncpy(parsed.latest_version, token, sizeof(parsed.latest_version) - 1);
    parsed.latest_version[sizeof(parsed.latest_version) - 1] = '\0';

    return true; // 成功解析
}

static esp_err_t ota_http_event_handler(esp_http_client_event_t *evt) {
    static int binary_file_length = 0;
    static int total_length = 0;
    static float last_reported_progress = 0;  // 上一次报告的进度
    const float progress_threshold = 1.0;     // 设定进度的变化阈值(1%)

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE("ota", "HTTP_EVENT_ERROR");
            binary_file_length = 0;
            total_length = 0;
            last_reported_progress = 0;
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("ota", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("ota", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI("ota", "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            if (strcmp(evt->header_key, "Content-Length") == 0) {
                total_length = atoi(evt->header_value);
            }
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                binary_file_length += evt->data_len;
                if (total_length > 0) {
                    float progress = (binary_file_length * 100.0) / total_length;
                    if (progress - last_reported_progress >= progress_threshold) {
                        ESP_LOGI("ota", "Download progress: %.2f%%", progress);
                        lv_label_set_text_fmt(ui_OTA_Progress_Text, "%d%%", (int)progress);
                        lv_bar_set_value(ui_OTA_Progress_Bar, progress, LV_ANIM_OFF);
                        last_reported_progress = progress;
                    }
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI("ota", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("ota", "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI("ota", "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}
// 显示ota进度条
void ota_msg_cb(void *param) {
    lv_obj_clear_flag(ui_OTA_Progress, LV_OBJ_FLAG_HIDDEN);
}
void ota_task(void *pvParameter)
{
    ESP_LOGI("ota", "Starting OTA example task");
    esp_http_client_config_t config;
    memset(&config, 0, sizeof(config));

    config.url = parsed.ota_url;
    config.crt_bundle_attach = esp_crt_bundle_attach; 
    config.event_handler = ota_http_event_handler;
    config.keep_alive_enable = true;
    config.skip_cert_common_name_check = true;
    config.use_global_ca_store = true;

    esp_https_ota_config_t ota_config;
    memset(&ota_config, 0, sizeof(ota_config));
    ota_config.http_config = &config;
    ESP_LOGI("ota", "Attempting to download update from %s", config.url);

    lv_async_call(ota_msg_cb, NULL);
    
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI("ota", "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE("ota", "Firmware upgrade failed");
        vTaskDelete(NULL);
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        esp_mqtt_client_subscribe(client, full_topic, 0);
        printf("已订阅: %s\n", full_topic);

        esp_mqtt_client_subscribe(client, "/XZBGS3DOWN/FFFF", 0);
        printf("已订阅: /XZBGS3DOWN/FFFF\n");
        break;
    case MQTT_EVENT_DATA:
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        if (strncmp(event->topic, full_topic, event->topic_len) == 0 || strncmp(event->topic, "/XZBGS3DOWN/FFFF", event->topic_len) == 0) {
            printf("验证通过\n");
            if (parse_data(event->data)) {

                printf("Force Update: %s\n", parsed.force_update ? "true" : "false");
                printf("Command Type: %s\n", parsed.cmd_type);
                printf("OTA URL: %s\n", parsed.ota_url);
                printf("OTA Version: %s\n", parsed.ota_ver);
                printf("Device Name: %s\n", parsed.device_name);
                printf("Latest Version: %s\n", parsed.latest_version);

                if (parsed.force_update && strcmp(parsed.cmd_type, "bg3ota") == 0
                    && strcmp(parsed.device_name, "XZ-BGS3-A") == 0
                    && strcmp(parsed.ota_ver, esp32_version) != 0) {

                    xTaskCreate(ota_task, "ota", 5120, NULL, 5, NULL);
                } else {
                    ESP_LOGE(TAG, "OTA 未启动:\nforce_update: %d\nstrcmp(parsed.cmd_type, bg3ota): %d\nstrcmp(parsed.device_name, XZ-BGS3-A): %d\nstrcmp(parsed.ota_ver, esp32_version): %d",
                    parsed.force_update, strcmp(parsed.cmd_type, "bg3ota"), strcmp(parsed.device_name, "XZ-BGS3-A"), strcmp(parsed.ota_ver, esp32_version));
                }
            } else {
                ESP_LOGE(TAG, "解析错误");
            }
        } else {
            ESP_LOGE(TAG, "验证失败: %s", event->topic);
        }
        break;
    default:
        break;
    }
}

void mqtt_app_start(void * param) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://xiaozhuiot.cn:1883",
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
        .credentials.username = "xiaozhu",
        .credentials.authentication.password = "ieTOIugSDfieWw23gfiwefg",
        .task.stack_size = 4096
    };

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(str_mac, sizeof(str_mac), "%02X%02X%02X%02X%02X%02X", 
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    snprintf(full_topic, sizeof(full_topic), "/XZBGS3DOWN/%s", str_mac);

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    vTaskDelete(NULL);
}
void mqtt_app_stop(void) {
    esp_mqtt_client_stop(client);
}
void mqtt_app_cleanup(void) {
    esp_mqtt_client_destroy(client);
}

void mqtt_publish_message(const char *message, int qos, int retain) {
    char up_topic[40];
    snprintf(up_topic, sizeof(up_topic), "/XZBGS3UP/%s", str_mac);
    int msg_id = esp_mqtt_client_publish(client, up_topic, message, strlen(message), qos, retain);
    ESP_LOGI(TAG, "Message sent, msg_id=%d, topic=%s, message=%s", msg_id, up_topic, message);
}
