#ifndef TOUCH_H
#define TOUCH_H

#include "../lvgl/lvgl.h"

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
extern lv_indev_t *indev_touch;

void disabled_touch(void);
void enable_touch(void);
#endif

#endif // TOUCH_H