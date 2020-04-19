#ifndef __engine_load_leds__
#define __engine_load_leds__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_STRIP_LATCH_GPIO	32
#define LED_STRIP_DATA_GPIO	25
#define LED_STRIP_CLOCK_GPIO	33

#define LED_STRIP_RED		12 /* not controlled by SN74HC595N shift register */

extern void engine_load_init();

/**
 * load value can be: 0-9
 */
extern void led_strip_set(int load);

/**
 * Plays animation when device is initialized
 */
extern void init_animation();

#endif