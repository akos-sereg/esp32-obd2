#ifndef __app_main_h_included__
#define __app_main_h_included__

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "rom/uart.h"

#include "engine-load-leds.h"
#include "switches.h"
#include "lcd.h"
#include "bluetooth.h"
#include "protocol.h"
#include "state.h"

#endif