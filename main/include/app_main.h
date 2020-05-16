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
#define AVERAGE_FUEL_CONSUMPTION_PER_100_KM    7.2

// baseline RPM: the RPM value once your car warmed up, at neutral, when you are not pushing the gas pedal
// this value is used when calculating LED strip value (eg. how many leds should be ON) - we do not want
// to set 0-9 value from 0 RPM to eg. 4200 RPM, because from 0-900 RPM we usually we will never receive data
#define BASELINE_RPM 900

// this is not the max RPM that your engine can manage, but the max that you usually reach at normal
// usage. this number is used when calculating when to enable all LEDs on led strip
#define MAX_RPM_AT_NORMAL_USAGE 3600

// if this value is set to 0, led strip will display Engine Load
#define LED_STRIP_DISPLAYS_RPM   0

#endif