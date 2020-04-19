#include "include/app_main.h"
#include <inttypes.h>


/**
W (20619) BT_RFCOMM: port_rfc_closed RFCOMM connection in state 2 closed: Peer connection failed (res: 16)
I (20619) SPP_INITIATOR_DEMO: ESP_SPP_CLOSE_EVT
*/
void main_task(void * pvParameter)
{
    int cnt = 0;
    int tick_rate_ms = 50;
    int64_t now;
    int is_lcd_value_request = 0;
    int is_lcd_request_sent = 0;

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
    init_animation();

    while(1) {
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
            lcd_display_text("Connected.", "");
            led_strip_set(0);
            app_state.obd2_bluetooth.displaying_connected = 1;
            app_state.obd2_bluetooth.displayed_connected = 1;
        }

        // connected to bluetooth, retrieve engine load periodically
        if (app_state.obd2_bluetooth.is_connected) {
            now = get_epoch_milliseconds();

            if ((get_time_last_lcd_data_received() + BT_LCD_DATA_POLLING_INTERVAL) < now
                && !bt_waiting_for_response) {
                is_lcd_value_request = 1;
            }

            // keep polling when applicable - last response already processed, poll interval elapsed
            if ((bt_get_last_request_sent() + BT_ENGINE_LOAD_POLL_INTERVAL) < now
                && bt_response_processed) {

                if (is_lcd_value_request) {

                    is_lcd_request_sent = 1;
                    bt_send_data(get_lcd_page_obd_code()); // OBD PID of current page displayed by LCD
                } else {
                    bt_send_data(LED_STRIP_DISPLAYS_RPM ? obd2_request_rpm() : obd2_request_calculated_engine_load());
                }
            }


            // process incoming data
            if (!bt_response_processed && bt_response_data_len > 0) {
                remove_char(bt_response_data, '\n');
                remove_char(bt_response_data, '\r');
                handle_obd2_response(bt_response_data, is_lcd_value_request && is_lcd_request_sent);

                if (is_lcd_value_request && is_lcd_request_sent) {
                    reset_time_last_lcd_data_received();
                    is_lcd_value_request = 0;
                    is_lcd_request_sent = 0;
                }

                bt_response_processed = 1;
            }


            // restart polling if OBD2 did not respond for a while
            // not sure this is needed - in case led strip got stuck, this might be helpful
            if (!bt_response_data_len
                && !bt_response_processed
                && (bt_get_last_request_sent() + BT_RESTART_POLLING_ENGINE_LOAD_AFTER) < now) {
                // bt_send_data(obd2_request_calculated_engine_load());
            }
        }

        // connected to bluetooth OBD2 already, displaying data - one time refresh LCD
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
            lcd_display_text("Restarting ...", "");
            nvs_shutdown();
            esp_restart();
        }

        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }
}

void app_main()
{
    init_bluetooth();
    init_nvs_store();

    // load LCD display mode (page) from NVS memory - refresh_lcd_display() will show
    // the correct page when it is called
    LCD_DISPLAY_MODE = get_nvs_value(NVS_KEY_MODE);

    reset_app_state();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

