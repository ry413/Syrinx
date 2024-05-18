/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include "st7701s.h"

// #include "../ui/ui.h"
#include "../uis/ui.h"
#include "wifi.h"
#include "backlight.h"
#include "nvs_flash.h"
#include "driver/uart.h"


#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_demos.h"
#else
#include "lvgl/lvgl.h"
#include "demos/lv_demos.h"
#endif


#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
#include "driver/i2c.h"
#if CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_GT911
#include "esp_lcd_touch_gt911.h"
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_TT21100
#include "esp_lcd_touch_tt21100.h"
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT5X06
#include "esp_lcd_touch_ft5x06.h"
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT6X06
#include "esp_lcd_touch_ft6x06.h"
#endif
#include <nvs.h>
#endif

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if	defined(CONFIG_EXAMPLE_PANEL_ZX3D95CE01S_UR)
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

#define EXAMPLE_PIN_NUM_BK_LIGHT       (1)

#define EXAMPLE_PIN_NUM_PCLK     (41)
#define EXAMPLE_PIN_NUM_DE       (-1)
#define EXAMPLE_PIN_NUM_VSYNC    (39)
#define EXAMPLE_PIN_NUM_HSYNC    (38)

#define EXAMPLE_PIN_NUM_DATA0    (0)  // B0
#define EXAMPLE_PIN_NUM_DATA1    (45)  // B1
#define EXAMPLE_PIN_NUM_DATA2    (48)  // B2
#define EXAMPLE_PIN_NUM_DATA3    (47)  // B3
#define EXAMPLE_PIN_NUM_DATA4    (21)  // B4
#define EXAMPLE_PIN_NUM_DATA5    (14)  // G0
#define EXAMPLE_PIN_NUM_DATA6    (13)  // G1
#define EXAMPLE_PIN_NUM_DATA7    (12)  // G2
#define EXAMPLE_PIN_NUM_DATA8    (11)  // G3
#define EXAMPLE_PIN_NUM_DATA9    (10)  // G4
#define EXAMPLE_PIN_NUM_DATA10   (9)  // G5
#define EXAMPLE_PIN_NUM_DATA11   (46)  // R0
#define EXAMPLE_PIN_NUM_DATA12   (3)   // R1
#define EXAMPLE_PIN_NUM_DATA13   (8)   // R2
#define EXAMPLE_PIN_NUM_DATA14   (18)  // R3
#define EXAMPLE_PIN_NUM_DATA15   (17)   // R4

#define EXAMPLE_PIN_NUM_DISP_EN        (-1)
#define EXAMPLE_PIN_NUM_RST            (-1) // LCD RESET
#elif defined(CONFIG_EXAMPLE_PANEL_ZX3D95CE01S_AR)
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

#define EXAMPLE_PIN_NUM_BK_LIGHT       (1)

#define EXAMPLE_PIN_NUM_PCLK    	   (14)
#define EXAMPLE_PIN_NUM_DE       	   (13)
#define EXAMPLE_PIN_NUM_VSYNC    	   (12)
#define EXAMPLE_PIN_NUM_HSYNC    	   (11)

#define EXAMPLE_PIN_NUM_DATA0          (10)  // B0
#define EXAMPLE_PIN_NUM_DATA1          (9)  // B1
#define EXAMPLE_PIN_NUM_DATA2          (40)  // B2
#define EXAMPLE_PIN_NUM_DATA3          (20)  // B3
#define EXAMPLE_PIN_NUM_DATA4          (19)  // B4
#define EXAMPLE_PIN_NUM_DATA5          (41)  // G0
#define EXAMPLE_PIN_NUM_DATA6          (46)  // G1
#define EXAMPLE_PIN_NUM_DATA7          (3)  // G2
#define EXAMPLE_PIN_NUM_DATA8          (42)  // G3
#define EXAMPLE_PIN_NUM_DATA9          (8)  // G4
#define EXAMPLE_PIN_NUM_DATA10         (18)  // G5
#define EXAMPLE_PIN_NUM_DATA11         (2)  // R0
#define EXAMPLE_PIN_NUM_DATA12         (17)   // R1
#define EXAMPLE_PIN_NUM_DATA13         (16)   // R2
#define EXAMPLE_PIN_NUM_DATA14         (1)  // R3
#define EXAMPLE_PIN_NUM_DATA15         (15)   // R4

#define EXAMPLE_PIN_NUM_DISP_EN        (-1)
#define EXAMPLE_PIN_NUM_RST            (-1) // LCD RESET
#else
#error "not support hardware"
#endif


// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              480
#define EXAMPLE_LCD_V_RES              480

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
#if	defined(CONFIG_EXAMPLE_PANEL_ZX3D95CE01S_UR)
#define EXAMPLE_I2C_NUM                 0   // I2C number
#define EXAMPLE_I2C_SCL                 15
#define EXAMPLE_I2C_SDA                 16
#elif defined(CONFIG_EXAMPLE_PANEL_ZX3D95CE01S_AR)
#define EXAMPLE_I2C_NUM                 0   // I2C number
#define EXAMPLE_I2C_SCL                 6
#define EXAMPLE_I2C_SDA                 7
#else
#error "not support hardware"
#endif
#endif

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

// we use two semaphores to sync the VSYNC event and the LVGL task, to avoid potential tearing effect
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
SemaphoreHandle_t sem_vsync_end;
SemaphoreHandle_t sem_gui_ready;
#endif
static void create_demo_application(void);

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
static void example_lvgl_touch_cb(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    /* Read touch controller data */
    esp_lcd_touch_read_data(drv->user_data);

    /* Get coordinates */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(drv->user_data, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PR;
        ESP_LOGI(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
#endif

static bool example_on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE) {
        xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    }
#endif
    return high_task_awoken == pdTRUE;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
//    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
//    int offsetx1 = area->x1;
//    int offsetx2 = area->x2;
//    int offsety1 = area->y1;
//    int offsety2 = area->y2;
//#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
//    xSemaphoreGive(sem_gui_ready);
//    xSemaphoreTake(sem_vsync_end, portMAX_DELAY);
//#endif
//    // pass the draw buffer to the driver
//    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
//    lv_disp_flush_ready(drv);

	esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
	xSemaphoreGive(sem_gui_ready);
	xSemaphoreTake(sem_vsync_end, portMAX_DELAY);
#endif
	// pass the draw buffer to the driver
	esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
	lv_disp_flush_ready(drv);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void lvgl_task(void *pvParameter)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    ESP_LOGI(TAG, "Create semaphores");
    sem_vsync_end = xSemaphoreCreateBinary();
    assert(sem_vsync_end);
    sem_gui_ready = xSemaphoreCreateBinary();
    assert(sem_gui_ready);
#endif

#if EXAMPLE_PIN_NUM_RST>=0
    ESP_LOGI(TAG, "reset pin");

//	gpio_pad_select_gpio(EXAMPLE_PIN_NUM_RST);
//	gpio_set_direction(EXAMPLE_PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_config_t rst_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_RST,
		.pull_up_en=GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&rst_gpio_config));

	//Reset the display
	gpio_set_level(EXAMPLE_PIN_NUM_RST, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(EXAMPLE_PIN_NUM_RST, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);
#endif

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif

    st7701s_init();

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
#if CONFIG_EXAMPLE_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 10 * EXAMPLE_LCD_H_RES,
#endif
		//.on_frame_trans_done=example_on_vsync_event,
        .clk_src = LCD_CLK_SRC_PLL240M,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
            EXAMPLE_PIN_NUM_DATA8,
            EXAMPLE_PIN_NUM_DATA9,
            EXAMPLE_PIN_NUM_DATA10,
            EXAMPLE_PIN_NUM_DATA11,
            EXAMPLE_PIN_NUM_DATA12,
            EXAMPLE_PIN_NUM_DATA13,
            EXAMPLE_PIN_NUM_DATA14,
            EXAMPLE_PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            // The following parameters should refer to LCD spec

//            .hsync_back_porch = 80,
//            .hsync_front_porch = 80,
//            .hsync_pulse_width = 20,
//            .vsync_back_porch = 20,
//            .vsync_front_porch = 120,
//            .vsync_pulse_width = 10,

//            .hsync_back_porch = 40,
//            .hsync_front_porch = 8,
//            .hsync_pulse_width = 10,
//            .vsync_back_porch = 40,
//            .vsync_front_porch = 8,
//            .vsync_pulse_width = 10,

            .hsync_back_porch = 1,
            .hsync_front_porch = 4,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 10,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,

            .flags.pclk_active_neg = false,
        },
        .flags.fb_in_psram = true, // allocate frame buffer in PSRAM
#if CONFIG_EXAMPLE_DOUBLE_FB
        .flags.double_fb = true,   // allocate double frame buffer
#endif // CONFIG_EXAMPLE_DOUBLE_FB
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = example_on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
    esp_lcd_touch_handle_t tp = NULL;
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;

    ESP_LOGI(TAG, "Initialize I2C");

    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = EXAMPLE_I2C_SDA,
        .scl_io_num = EXAMPLE_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    /* Initialize I2C */
    ESP_ERROR_CHECK(i2c_param_config(EXAMPLE_I2C_NUM, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(EXAMPLE_I2C_NUM, i2c_conf.mode, 0, 0, 0));

#if CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_GT911
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_TT21100
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_TT21100_CONFIG();
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT5X06
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT6X06
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT6x06_CONFIG();
#endif

    ESP_LOGI(TAG, "Initialize touch IO (I2C)");
    /* Touch IO handle */
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)EXAMPLE_I2C_NUM, &tp_io_config, &tp_io_handle));
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = EXAMPLE_LCD_V_RES,
        .y_max = EXAMPLE_LCD_H_RES,
        .rst_gpio_num = -1,
        .int_gpio_num = -1,
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    /* Initialize touch */
#if CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_GT911
    ESP_LOGI(TAG, "Initialize touch controller GT911");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &tp));
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_TT21100
    ESP_LOGI(TAG, "Initialize touch controller TT21100");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_tt21100(tp_io_handle, &tp_cfg, &tp));
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT5X06
    ESP_LOGI(TAG, "Initialize touch controller FT5X06");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &tp));
#elif CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_FT6X06
    ESP_LOGI(TAG, "Initialize touch controller FT5X06");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft6x06(tp_io_handle, &tp_cfg, &tp));
#endif
#endif

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_EXAMPLE_DOUBLE_FB
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // initialize LVGL draw buffers
    lv_disp_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES);
#else
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    //缓存视实际项目使用MALLOC_CAP_SPIRAM或MALLOC_CAP_DMA
    //优先使用内部MALLOC_CAP_DMA 刷屏会提高一个档次
    //优先使用双buffer

    // 在分配前打印可用内存
    // ESP_LOGI("LVGL MALLOC1: ", "Free internal memory before allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // ESP_LOGI("LVGL MALLOC1: ", "Free DMA memory before allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    // ESP_LOGI("LVGL MALLOC1: ", "Free SPIRAM memory before allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 120 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);//MALLOC_CAP_SPIRAM
    assert(buf1);
    buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 120 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);//MALLOC_CAP_DMA
    assert(buf2);

    // ESP_LOGI("LVGL MALLOC2: ", "Free internal memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // ESP_LOGI("LVGL MALLOC2: ", "Free DMA memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    // ESP_LOGI("LVGL MALLOC2: ", "Free SPIRAM memory after allocation: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * 120);
#endif // CONFIG_EXAMPLE_DOUBLE_FB

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
#if CONFIG_EXAMPLE_DOUBLE_FB
    disp_drv.full_refresh = true; // the full_refresh mode can maintain the synchronization between the two frame buffers
#endif
    lv_disp_t *disp=lv_disp_drv_register(&disp_drv);

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
    static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = example_lvgl_touch_cb;
    indev_drv.user_data = tp;

    lv_indev_drv_register(&indev_drv);
#endif

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    // CREATE Application
    create_demo_application();

	while (1)
	{
		/* Delay 1 tick (assumes FreeRTOS tick is 10ms */
		vTaskDelay(pdMS_TO_TICKS(10));

		/* Try to take the semaphore, call lvgl related function on success */
		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
		{
			lv_task_handler();
			xSemaphoreGive(xGuiSemaphore);
		}
	}
}

static void create_demo_application(void)
{
    ui_init();
    initBacklight();
// #if defined CONFIG_LV_USE_DEMO_WIDGETS
//     lv_demo_widgets();
// #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
//     lv_demo_keypad_encoder();
// #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
//     lv_demo_benchmark();
// #elif defined CONFIG_LV_USE_DEMO_STRESS
//     lv_demo_stress();
// #else
//     #error "No demo application selected."
// #endif
}


#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
char pWriteBuffer[1024];
#endif
static void monitor_task(void *pvParameter)
{
	while(1)
	{
		vTaskDelay(5000 / portTICK_PERIOD_MS);
    	ESP_LOGI(TAG,"-INTERNAL RAM left %dKB",heap_caps_get_free_size( MALLOC_CAP_INTERNAL )/1024);
        ESP_LOGI(TAG, "-     DMA left %dkB",heap_caps_get_free_size( MALLOC_CAP_DMA )/1024);
		#ifdef CONFIG_SPIRAM
    	ESP_LOGI(TAG,"-     SPI RAM left %dkB",heap_caps_get_free_size( MALLOC_CAP_SPIRAM )/1024);
		#endif
	    #ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
		vTaskList((char *)&pWriteBuffer);
		printf("task_name   task_state  priority   stack  tasK_num\n");
		printf("%s\r\n", pWriteBuffer);
		#endif
	}
}

#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        GPIO_NUM_43
#define UART_RX_PIN        GPIO_NUM_44
#define BUF_SIZE           1024
void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_LOGI(TAG, "UART initialized successfully");
}
void send_at_command(const char* command) {
    char command_with_newline[128];
    snprintf(command_with_newline, sizeof(command_with_newline), "%s\r\n", command);

    int len = uart_write_bytes(UART_PORT_NUM, command_with_newline, strlen(command_with_newline));
    if (len < 0) {
        ESP_LOGE(TAG, "UART write error");
    } else {
        ESP_LOGI(TAG, "Sent AT command: %s", command);
    }
}
void receive_at_response() {
    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0'; // Null-terminate the response
            ESP_LOGI(TAG, "Received response: %s", data);

            // 解析响应
            if (strstr((char*)data, "ERR") != NULL) {
                ESP_LOGE(TAG, "Error response received: %s", data);
            } else if (strstr((char*)data, "OK") != NULL) {
                ESP_LOGI(TAG, "Success response received: %s", data);
            } else {
                ESP_LOGW(TAG, "Unhandled response: %s", data);
            }

            // 检查响应是否完整
            if (strstr((char*)data, "\r\n") != NULL) {
                ESP_LOGI(TAG, "Complete response received");
            }
        } else if (len < 0) {
            ESP_LOGE(TAG, "UART read error");
        } else {
            ESP_LOGW(TAG, "No response received");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay to avoid busy loop
    }
}
/**********************
 *   APPLICATION MAIN
 **********************/
void app_main() {
    // 检查NVS是否已初始化
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NVS已初始化");
    } else {
        ESP_LOGE(TAG, "NVS初始化失败: %s", esp_err_to_name(ret));
    }
	//lvgl的任务优先级不能是0 也不能高 高了影响wifi其他性能,0优先级刷新fps低
    //xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 4096*2, NULL, 2, NULL, 1);
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 2, NULL);
    xTaskCreate(monitor_task, "monitor", 4096, NULL, 1, NULL);
    xTaskCreate(wifi_task, "wifi_task", 4096, NULL, 5, NULL);

    uart_init();

    // 创建任务接收 AT 响应
    xTaskCreate(receive_at_response, "receive_at_response", 4096, NULL, 5, NULL);

    // 发送 AT 指令
    send_at_command("AT"); // 发送获取固件版本的 AT 指令

    // 为了确保有足够时间接收响应，可以在这里添加延迟
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}
