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
#include "include/state.h"

#define TAG "app"

void main_task(void * pvParameter)
{
  int cnt = 0;
  int tick_rate_ms = 50;

  app_state.obd2_bluetooth.displaying_connected = 0;
  app_state.obd2_bluetooth.displaying_connected_elapsed_ms = 0;
  app_state.obd2_bluetooth.displaying_connecting_elapsed_ms = 0;
  app_state.obd2_bluetooth.displayed_connected = 0;
  app_state.obd2_bluetooth.is_connected = 0;

  engine_load_init();
  setup_switches();
  i2c_master_init();

  lcd_display_text("Connecting to", "Bluetooth OBD2");

  while(1) {
    engine_load_set(cnt);
    cnt++;

    if (cnt == 10) {
      cnt = 0;
      vTaskDelay(500 / portTICK_RATE_MS);
      if (app_state.obd2_bluetooth.displaying_connected) {
        app_state.obd2_bluetooth.displaying_connected_elapsed_ms += 500;
      }
      if (!app_state.obd2_bluetooth.is_connected) {
        app_state.obd2_bluetooth.displaying_connecting_elapsed_ms += 500;
      }
    }

    // connected to bluetooth, notify user on display for a couple of seconds
    if (app_state.obd2_bluetooth.is_connected
      && !app_state.obd2_bluetooth.displaying_connected
      && !app_state.obd2_bluetooth.displayed_connected) {
        lcd_display_text("Connected to", "OBD2");
        app_state.obd2_bluetooth.displaying_connected = 1;
        app_state.obd2_bluetooth.displayed_connected = 1;
    }

    // connected to bluetooth OBD2, display data
    if (app_state.obd2_bluetooth.displaying_connected) {
      app_state.obd2_bluetooth.displaying_connected_elapsed_ms += 50;
      if (app_state.obd2_bluetooth.displaying_connected_elapsed_ms > 3000) {
        app_state.obd2_bluetooth.displaying_connected = 0;
        refresh_lcd_display();
      }
    }

    if (!app_state.obd2_bluetooth.is_connected) {
        app_state.obd2_bluetooth.displaying_connecting_elapsed_ms += tick_rate_ms;
    } else {
        app_state.obd2_bluetooth.displaying_connecting_elapsed_ms = 0;
    }

    if (app_state.obd2_bluetooth.displaying_connecting_elapsed_ms > 10000) {
        lcd_display_text("Connecting to", "OBD2 (2)");
        esp_restart();
    }

    vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
  }
}

void app_main()
{
    init_bluetooth();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

