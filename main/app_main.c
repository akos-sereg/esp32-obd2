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
#include "include/protocol.h"

#define TAG "app"

void main_task(void * pvParameter)
{
    int cnt = 0;
    int tick_rate_ms = 50;
    int obd2_connected_displaying = 0;
    int obd2_connected_displayed = 0;
    int obd2_connected_displaying_elapsed_ms = 0;
    int obd2_connecting_elapsed_ms = 0;

    engine_load_init();
    setup_switches();
    i2c_master_init();
    // refresh_lcd_display();

    lcd_display_text("Connecting to", "Bluetooth OBD2");

    while(1) {
	engine_load_set(cnt);
	cnt++;

	if (cnt == 10) {
	    cnt = 0;
	    vTaskDelay(500 / portTICK_RATE_MS);
	    if (obd2_connected_displaying) {
		obd2_connected_displaying_elapsed_ms += 500;
	    }
	    if (!bt_is_connected) {
		obd2_connecting_elapsed_ms += 500;
	    }
	}

	// connected to bluetooth, notify user on display for a couple of seconds
	if (bt_is_connected && !obd2_connected_displaying && !obd2_connected_displayed) {
	    lcd_display_text("Connected to", "OBD2");
	    obd2_connected_displaying = 1;
	    obd2_connected_displayed = 1;
	}

	// connected to bluetooth OBD2, display data
	if (obd2_connected_displaying) {
	    obd2_connected_displaying_elapsed_ms += 50;
	    if (obd2_connected_displaying_elapsed_ms > 3000) {
		obd2_connected_displaying = 0;
		refresh_lcd_display();
	    }
	}

	if (!bt_is_connected) {
	    obd2_connecting_elapsed_ms += tick_rate_ms;
	}

	if (obd2_connecting_elapsed_ms > 10000) {
	    lcd_display_text("Connecting to", "OBD2 (2)");
	    init_bluetooth();
	    //lcd_display_text("Connected to", "OBD2 (2)");
	}

	vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }
}

void app_main()
{
    init_bluetooth();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

