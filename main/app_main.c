#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "rom/uart.h"

#include "include/engine-load-leds.h"
#include "include/switches.h"
#include "include/lcd.h"
#include "include/bluetooth.h"

#define TAG "app"

void main_task(void * pvParameter)
{
    int cnt = 0;
    engine_load_init();
    setup_switches();
    i2c_master_init();
    refresh_lcd_display();

    // initialize bluetooth connection for OBD2 communication
    if (CONNECT_BLUETOOTH) {
	init_bluetooth();
    }

    while(1) {
	engine_load_set(cnt);
	cnt++;
	if (cnt == 10) {
	    cnt = 0;
	    vTaskDelay(500 / portTICK_RATE_MS);
	}

	vTaskDelay(50 / portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

