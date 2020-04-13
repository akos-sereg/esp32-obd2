#include "include/app_main.h"

// #define TAG "app"

void main_task(void * pvParameter)
{
    int cnt = 0;
    int tick_rate_ms = 50;

    // initializing app state
    // see include/state.h fore more details about state fields
    app_state.obd2_bluetooth.displaying_connected = 0;
    app_state.obd2_bluetooth.displaying_connected_elapsed_ms = 0;
    app_state.obd2_bluetooth.displaying_connecting_elapsed_ms = 0;
    app_state.obd2_bluetooth.displayed_connected = 0;
    app_state.obd2_bluetooth.is_connected = 0;

    // initializing LCD, Bluetooth, Input switch
    engine_load_init();
    setup_switches();
    i2c_master_init();

    lcd_display_text("Connecting to", "Bluetooth OBD2");

    while(1) {
        // engine_load_set(cnt);
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

        // connected to bluetooth OBD2 already, displaying data
        if (app_state.obd2_bluetooth.displaying_connected) {
            app_state.obd2_bluetooth.displaying_connected_elapsed_ms += 50;
            if (app_state.obd2_bluetooth.displaying_connected_elapsed_ms > 3000) {
                app_state.obd2_bluetooth.displaying_connected = 0;
                refresh_lcd_display();
            }
        }

        // connecting to bluetooth
        if (!app_state.obd2_bluetooth.is_connected) {
            app_state.obd2_bluetooth.displaying_connecting_elapsed_ms += tick_rate_ms;
        } else {
            app_state.obd2_bluetooth.displaying_connecting_elapsed_ms = 0;
        }

        if (app_state.obd2_bluetooth.displaying_connecting_elapsed_ms > 15000) {
            // could not connect to bluetooth, or connection is lost for 15 seconds
            // leaving a message on LCD display and rebooting esp32 device (restart tro reconnect)
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

