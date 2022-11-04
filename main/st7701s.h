#ifndef __ST7701S__
#define __ST7701S__

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"



void st7701s_init(void);

#ifdef __cplusplus
	}
#endif
#endif /* __ST7701S__ */
