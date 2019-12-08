#ifndef __engine_load_leds__
#define __engine_load_leds__

#include "driver/gpio.h"

#define LED_STRIP_LATCH_GPIO	32
#define LED_STRIP_DATA_GPIO	25
#define LED_STRIP_CLOCK_GPIO	33

#define LED_STRIP_RED		12 /* not controlled by SN74HC595N shift register */

extern void engine_load_init();
extern void engine_load_set(int);

#endif