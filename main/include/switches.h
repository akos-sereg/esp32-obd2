#ifndef __switches_h_included__
#define __switches_h_included__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "nvs-store.h"
#include "protocol.h"

#define GPIO_INPUT_IO	    34
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO)
#define ESP_INTR_FLAG_DEFAULT 0
#define LONG_KEYPRESS_INTERVAL_MS   1200 // holding the button in pressed state in this amount of time (ms) is considered as a long key press

extern int SWITCH_1_STATE;
extern int LCD_DISPLAY_MODE;
extern int MAX_LCD_DISPLAY_MODE;

extern void listen_switches(void* arg);
extern void setup_switches();

#endif
