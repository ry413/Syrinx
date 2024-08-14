#include "rs485.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "string.h"
#include "../../ui/ui.h"
#include "rs485.h"

static const char *TAG = "rs485";

TaskHandle_t bath_play_task_handle = NULL;
TaskHandle_t music_play_task_handle = NULL;
TaskHandle_t nature_play_task_handle = NULL;
static SemaphoreHandle_t rs485_handle_semaphore = NULL; // 防止同时处理多条485




int current_bath_playing_index = 0;

typedef struct {
    uint8_t header;
    uint8_t command[5];
    uint8_t checksum;
    uint8_t footer;
} rs485_packet_t;

esp_err_t rs485_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS, 
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    ESP_ERROR_CHECK(uart_param_config(RS485_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(RS485_UART_PORT, RS485_TX_GPIO_NUM, RS485_RX_GPIO_NUM, RS485_DE_GPIO_NUM, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(RS485_UART_PORT, 1024 * 2, 0, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_set_mode(RS485_UART_PORT, UART_MODE_RS485_HALF_DUPLEX));

    ESP_LOGI(TAG, "UART 1 初始化成功");

    rs485_handle_semaphore = xSemaphoreCreateBinary();
    if (rs485_handle_semaphore == NULL) {
        ESP_LOGE("initActions", "创建信号量失败");
    }
    xSemaphoreGive(rs485_handle_semaphore);
    return ESP_OK;
}

void print_rs485_packet(const rs485_packet_t *packet) {
    printf("RS485 Packet:\n");
    printf("Header: 0x%02X\n", packet->header);
    printf("Command: ");
    for (int i = 0; i < 5; ++i) {
        printf("0x%02X ", packet->command[i]);
    }
    printf("\n");
    printf("Checksum: 0x%02X\n", packet->checksum);
    printf("Footer: 0x%02X\n", packet->footer);
}
// 检查校验和
uint8_t calculate_checksum(rs485_packet_t *packet) {
    uint8_t checksum = 0;
    checksum += packet->header;
    for (size_t i = 0; i < sizeof(packet->command); i++) {
        checksum += packet->command[i];
    }
    return checksum & 0xFF;
}
static void play_bath_music_with_index(int index) {
    AT_AF(bath_file_ids[index]);
}
void show_bath_sound_icon_callback(void *param) {
    lv_obj_clear_flag(ui_bath_sound_icon, LV_OBJ_FLAG_HIDDEN);
}
void hide_bath_sound_icon_callback(void *param) {
    lv_obj_add_flag(ui_bath_sound_icon, LV_OBJ_FLAG_HIDDEN);
}
// 下一首浴室音乐
void next_bath_track(void) {
    if (current_bath_playing_index < bath_files_count - 1) {
        current_bath_playing_index++;
    } else {
        current_bath_playing_index = 0;
    }
    play_bath_music_with_index(current_bath_playing_index);
}
// 上一首浴室音乐
void prev_bath_track(void) {
    if (current_bath_playing_index > 0) {
        current_bath_playing_index--;
    } else {
        current_bath_playing_index = bath_files_count - 1;
    }
    play_bath_music_with_index(current_bath_playing_index);
}
// 持续播放浴室音乐的任务
void play_bath_music_task(void *pvParameters) {
    while (1) {
        xEventGroupWaitBits(bt_event_group, EVENT_END_PLAY, pdTRUE, pdFALSE, portMAX_DELAY);
        next_bath_track();
    }
}
// 洗牌并创建浴室播放任务
void create_bath_play_task(void) {
    // 洗牌
    srand(global_time);
    for (int i = bath_files_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = bath_file_ids[i];
        bath_file_ids[i] = bath_file_ids[j];
        bath_file_ids[j]= temp;
    }
    if (bath_play_task_handle != NULL) {
        vTaskDelete(bath_play_task_handle);
        bath_play_task_handle = NULL;
    }
    xTaskCreate(play_bath_music_task, "play_bath_music_task", 4096, NULL, 5, &bath_play_task_handle);
    
}
// 判断指令类型并处理
void process_command(rs485_packet_t *packet, size_t len) {
    
    // 确保包长度正确
    if (len != PACKET_SIZE) {
        ESP_LOGE(TAG, "Invalid packet length: %d", len);
        return;
    }

    // 检查包头和包尾
    if (packet->header != PACKET_HEADER || packet->footer != PACKET_FOOTER) {
        ESP_LOGE(TAG, "Invalid packet header/footer: ");
        print_rs485_packet(packet);
        return;
    }

    // 检查校验和
    uint8_t calculated_chk = calculate_checksum(packet);
    if (calculated_chk != packet->checksum) {
        ESP_LOGE(TAG, "Checksum error: calculated 0x%02X, received 0x%02X", calculated_chk, packet->checksum);
        return;
    }

    // ******************** 指令 ********************

    // 插卡
    if (memcmp(packet->command, (uint8_t[]){0x80, 0x01, 0x00, 0x26, 0x01}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 插卡");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            set_backlight(backlight_level);
            // enable_touch();
            lv_obj_add_flag(ui_Disabled_Touch_Range, LV_OBJ_FLAG_HIDDEN);

            // clear一堆设置到默认状态
            reset_a_bunch_settings();
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 拔卡
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x01, 0x00, 0x26, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 拔卡");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            set_backlight(0);
            // disabled_touch();
            // 触摸缓冲好像删不掉, 所以开一个遮罩防止在拔卡后积累触摸事件
            lv_obj_clear_flag(ui_Disabled_Touch_Range, LV_OBJ_FLAG_HIDDEN);

            AT_CM(0);
            lv_scr_load(ui_Main_Window);  // 回到主界面理应会删掉音乐播放任务和自然之音播放任务, 浴室的在这里删
            if (bath_play_task_handle != NULL) {
                vTaskDelete(bath_play_task_handle);
                bath_play_task_handle = NULL;
                lv_async_call(hide_bath_sound_icon_callback, NULL);
            }
            // clear一堆设置到默认状态
            reset_a_bunch_settings();
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 睡眠模式
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x1E, 0x01, 0x0A, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 睡眠模式");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            if (lv_scr_act() == ui_Idle_Window) {
                offScreen(NULL);
            } else {
                sleep_mode();
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 明亮模式
    else if (memcmp(packet->command, (uint8_t[]){0x80, 0x1E, 0x01, 0x01, 0x00}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 明亮模式");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            onScreen(NULL);
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 播放
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x02}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 播放");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            lv_obj_t *scr = lv_scr_act();
            switch (work_mode)
            {
                case 0:
                    // 几个特殊界面
                    if (scr != ui_Music_Window && scr != ui_Music_Play_Window && scr != ui_Nature_Sound_Window && scr != ui_Bluetooth_WIndow) {
                        if (bath_play_task_handle == NULL) {
                            ESP_LOGI(TAG, "空闲模式, 非特殊界面, 未播放浴室音乐, 进音乐模式并打开通道并播放浴室音乐");
                            assert(music_play_task_handle == NULL && nature_play_task_handle == NULL);
                            AT_CM(2);
                            vTaskDelay(2000 / portTICK_PERIOD_MS);
                            open_bath_channel();
                            next_bath_track();
                            create_bath_play_task();
                            lv_async_call(show_bath_sound_icon_callback, NULL);
                        } else {
                            ESP_LOGI(TAG, "空闲模式, 非特殊界面, 正在播放浴室音乐, 拒绝播放指令");
                            assert(bath_play_task_handle != NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        }
                    } else {
                        ESP_LOGE(TAG, "空闲模式, 不应该在音乐库, 播放, 自然之音, 蓝牙界面之一");
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                    }
                    break;
                case 1:
                    // 蓝牙界面
                    if (scr == ui_Bluetooth_WIndow) {
                        if (bluetooth_state == 2) {
                            ESP_LOGI(TAG, "蓝牙模式, 蓝牙界面, 蓝牙已连接, 拒绝播放指令");
                        } else {
                            ESP_LOGI(TAG, "蓝牙模式, 蓝牙界面, 蓝牙未连接, 关闭功放并进入音乐模式并打开通道并播放浴室音乐");
                            AT_CL(0);
                            AT_CM(2);
                            open_bath_channel();
                            next_bath_track();
                            create_bath_play_task();
                            lv_async_call(show_bath_sound_icon_callback, NULL);
                            del_inactive_timer(); // 删除不活动定时器, 不再自动退出蓝牙界面
                        }
                    }
                    // 剩下所有界面
                    else {
                        ESP_LOGE(TAG, "蓝牙模式, 不应该在蓝牙界面之外");
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                    }
                    break;
                case 2:
                    // 音乐库界面
                    if (scr == ui_Music_Window) {
                        ESP_LOGI(TAG, "音乐模式, 音乐库界面, 拒绝播放指令");
                        assert(music_play_task_handle != NULL && bath_play_task_handle == NULL && nature_play_task_handle == NULL);
                    }
                    // 播放界面
                    else if (scr == ui_Music_Play_Window) {
                        ESP_LOGI(TAG, "音乐模式, 播放界面, 拒绝播放指令");
                        assert(music_play_task_handle != NULL && bath_play_task_handle == NULL && nature_play_task_handle == NULL);
                    }
                    // 自然之音界面
                    else if (scr == ui_Nature_Sound_Window) {
                        if (bath_play_task_handle == NULL && nature_play_task_handle == NULL) {
                            ESP_LOGI(TAG, "音乐模式, 自然之音界面, 未播放任何东西, 打开通道并播放浴室音乐");
                            open_bath_channel();
                            next_bath_track();
                            create_bath_play_task();
                            lv_async_call(show_bath_sound_icon_callback, NULL);
                        } else {
                            ESP_LOGI(TAG, "音乐模式, 自然之音界面, 正在播放某音乐, 拒绝播放指令");
                        }
                    }
                    // 蓝牙界面
                    else if (scr == ui_Bluetooth_WIndow) {
                        if (bluetooth_state == 2) {
                            ESP_LOGE(TAG, "音乐模式, 蓝牙界面, 蓝牙已连接, 这不应该");
                            assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        } else {
                            ESP_LOGI(TAG, "音乐模式, 蓝牙界面, 蓝牙未连接, 此时理应是在播放浴室音乐, 拒绝播放指令");
                            assert(bath_play_task_handle != NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        }
                    } else {
                        ESP_LOGI(TAG, "音乐模式, 非音乐库, 播放, 自然之音, 蓝牙界面之一, 此时应该在播放浴室音乐, 拒绝播放指令");
                    }
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 停止播放
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x03}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 停止");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            lv_obj_t *scr = lv_scr_act();
            switch (work_mode) {
            case 0:
            case 1:
                ESP_LOGI(TAG, "连音乐模式都不是, 拒绝停止指令");
                break;
            case 2:
                // 音乐库界面
                if (scr == ui_Music_Window) {
                    ESP_LOGI(TAG, "音乐模式, 音乐库界面, 拒绝停止指令");
                    assert(music_play_task_handle != NULL && bath_play_task_handle == NULL &&nature_play_task_handle == NULL);
                }
                // 播放界面
                else if (scr == ui_Music_Play_Window) {
                    ESP_LOGI(TAG, "音乐模式, 播放界面, 拒绝停止指令");
                    assert(music_play_task_handle != NULL && bath_play_task_handle == NULL &&nature_play_task_handle == NULL);
                }
                // 自然之音界面
                else if (scr == ui_Nature_Sound_Window) {
                    if (bath_play_task_handle != NULL) {
                        ESP_LOGI(TAG, "音乐模式, 自然之音界面, 正在播放浴室音乐, 仅停止播放");
                        assert(music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        AT_AA0();
                        vTaskDelete(bath_play_task_handle);
                        bath_play_task_handle = NULL;
                        lv_async_call(hide_bath_sound_icon_callback, NULL);
                    } else {
                        ESP_LOGI(TAG, "音乐模式, 自然之音界面, 未播放浴室音乐, 拒绝停止指令");
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL);
                    }
                }
                // 蓝牙界面
                else if (scr == ui_Bluetooth_WIndow) {
                    if (bath_play_task_handle != NULL) {
                        ESP_LOGI(TAG, "音乐模式, 蓝牙界面, 正在播放浴室音乐, 停止并回到蓝牙模式");
                        vTaskDelete(bath_play_task_handle);
                        bath_play_task_handle = NULL;
                        lv_async_call(hide_bath_sound_icon_callback, NULL);
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        open_living_room_channel();
                        AT_CM(1);
                        
                        create_inactive_timer();  // 回蓝牙模式后如果很久都不连接, 也要回主界面, 所以重建不活动定时器
                    } else {
                        ESP_LOGE(TAG, "音乐模式, 蓝牙界面, 也不在播放浴室音乐, 这不应该");
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                    }
                }
                // 别的界面
                else {
                    if (bath_play_task_handle != NULL) {
                        ESP_LOGI(TAG, "音乐模式, 非特殊界面, 正在播放浴室音乐, 停止并关闭功放并进入空闲模式");
                        vTaskDelete(bath_play_task_handle);
                        bath_play_task_handle = NULL;
                        lv_async_call(hide_bath_sound_icon_callback, NULL);
                        assert(bath_play_task_handle == NULL && music_play_task_handle == NULL && nature_play_task_handle == NULL);
                        AT_CL(0);
                        AT_CM(0);
                    }
                }
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 上一首
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x04}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 上一首");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            if (work_mode == 2) {
                if (bath_play_task_handle != NULL) {
                    prev_bath_track();
                } else if (music_play_task_handle != NULL) {
                    prevTrack(NULL);
                } else if (nature_play_task_handle != NULL) {
                    ESP_LOGI(TAG, "正在播放自然之音, 拒绝上一首指令");
                } else {
                    ESP_LOGE(TAG, "音乐模式, 但什么都不在播放");
                }
            } else {
                ESP_LOGI(TAG, "并不在音乐模式, 拒绝上一首指令");
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 下一首
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x05}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 下一首");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            if (work_mode == 2) {
                if (bath_play_task_handle != NULL) {
                    next_bath_track();
                } else if (music_play_task_handle) {
                    nextTrack(NULL);
                } else if (nature_play_task_handle != NULL) {
                    ESP_LOGI(TAG, "正在播放自然之音, 拒绝下一首指令");
                } else {
                    ESP_LOGE(TAG, "音乐模式, 但什么都不在播放");
                }
            } else {
                ESP_LOGI(TAG, "并不在音乐模式, 拒绝下一首指令");
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 播放/暂停
    else if (memcmp(packet->command, (uint8_t[]){0xA8, 0x00, 0x00, 0x00, 0x0C}, (size_t)5) == 0) {
        ESP_LOGI(TAG, "Command: 播放/暂停");
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            if (work_mode == 2) {
                if (bath_play_task_handle != NULL) {
                    playPause(NULL);
                } else if (music_play_task_handle != NULL) {
                    playPause(NULL);
                } else if (nature_play_task_handle != NULL) {
                    ESP_LOGI(TAG, "正在播放自然之音, 拒绝播放/暂停指令");
                } else {
                    ESP_LOGE(TAG, "音乐模式, 但什么都不在播放");
                }
            } else {
                ESP_LOGI(TAG, "并不在音乐模式, 拒绝播放/暂停指令");
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 查询MP
    else if (memcmp(packet->command, (uint8_t[]){0x16, 0xA0, 0x00, 0x02, 0x01}, (size_t)5) == 0) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        uint8_t cmd[] = {0x16, 0xA0, 0x00, 0x02, 0x01, 0x0a, 0x0d, 0x02};
        uart_write_bytes(RS485_UART_PORT, (const char*)cmd, sizeof(cmd));
        bluetooth_send_at_command("AT+MP", CMD_GET_PLAY_STATE);
        xEventGroupWaitBits(music_event_group, EVENT_GET_PLAY_STATE, pdTRUE, pdFALSE, portMAX_DELAY);
        printf("现在播放状态: %d\n", play_state);
    }
    
    // 时间同步
    else if (packet->command[0] == 0x78) {
        if (xSemaphoreTake(rs485_handle_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("已获得信号量\n");
            uint32_t timestamp = (packet->command[1] << 24) | (packet->command[2] << 16) |
                            (packet->command[3] << 8) | packet->command[4];
            // 使用时间戳进行后续处理
            printf("Received timestamp: %lu\n", timestamp);
            global_time = timestamp + 8 * 3600;

            // 启动定时器
            if (update_time_task_handle == NULL) {
                xTaskCreate(update_time_task, "updateTimeTask", 2048, NULL, 5, &update_time_task_handle);
            }
            xSemaphoreGive(rs485_handle_semaphore);
            printf("已释放信号量\n");
        }
    }
    // 未知指令
    else {
        // ESP_LOGE(TAG, "Unknown command");
    }
    // uart_write_bytes(RS485_UART_PORT, (const char *)packet, sizeof(rs485_packet_t)); // 谁写的这个?
}
void rs485_monitor_task(void *pvParameter) {
    rs485_packet_t packet;
    while (1) {
        int len = uart_read_bytes(RS485_UART_PORT, (uint8_t*)&packet, PACKET_SIZE, portMAX_DELAY);
        // print_rs485_packet(&packet);
        if (len > 0) {
            process_command(&packet, len);
        } else if (len < 0) {
            ESP_LOGE(TAG, "UART read error: %d", len);
        }
    }
}
