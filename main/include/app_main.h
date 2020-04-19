#ifndef __app_main_h_included__
#define __app_main_h_included__

#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "rom/uart.h"

#include "led-strip.h"
#include "switches.h"
#include "lcd.h"
#include "bluetooth.h"
#include "protocol.h"
#include "state.h"
#include "nvs-store.h"
#include "string-helper.h"
#include "command-handler.h"
#include "obd2.h"

#define FUEL_TANK_LITER                        55

// liter/100km - this one is for "distance to empty" calculation - use upper bound of real consumption
// to make sure that displayed "distance to empty" value is safe
#define AVERAGE_FUEL_CONSUMPTION_PER_100_KM    8

#define LED_STRIP_DISPLAYS_RPM   1

#endif