#include "include/command-handler.h"

/**
 * Calculations based on: https://en.wikipedia.org/wiki/OBD-II_PIDs
 *
 * Whitespaces are removed from payload!
 * eg. "NO DATA" is now "NODATA"
 * and "41 04 3E" is now "41043E"
 */
void handle_obd2_response(char *obd2_response) {
    // sample responses:
    //
    // Request  > 01 04                - Engine Load
    // Response > 41 04 3E             - or ...
    // Response > 41043E
    //
    // Request  > 01 2F                - Fuel Level
    // Response > 41 2F DE             - or ...
    // Response > 412FDE

    int a = -1; // first byte of response
    int b = -1; // second byte of response
    char *ptr;
    char hex_buf[3];

    if (strlen(obd2_response) >= 6
        && obd2_response[0] == 'N'
        && obd2_response[1] == 'O'
        && obd2_response[2] == 'D'
        && obd2_response[3] == 'A'
        && obd2_response[4] == 'T'
        && obd2_response[5] == 'A') {

        // NO DATA response, moving forward ...
        printf("  --> [OBD Response] NO DATA, moving forward");
        return;
    }

    // in case response is "410C1234", a = 12 in hex
    if (strlen(obd2_response) >= 6
        && ((obd2_response[4] >= '0' && obd2_response[4] <= '9') || (obd2_response[4] >= 'A' && obd2_response[4] <= 'F'))
        && ((obd2_response[5] >= '0' && obd2_response[5] <= '9') || (obd2_response[5] >= 'A' && obd2_response[5] <= 'F'))) {
        sprintf(hex_buf, "%c%c", obd2_response[4], obd2_response[5]);
        a = strtol(hex_buf, &ptr, 16);
    }

    // in case response is "410C1234", b = 34 in hex
    if (strlen(obd2_response) >= 8
        && ((obd2_response[6] >= '0' && obd2_response[6] <= '9') || (obd2_response[6] >= 'A' && obd2_response[6] <= 'F'))
        && ((obd2_response[7] >= '0' && obd2_response[7] <= '9') || (obd2_response[7] >= 'A' && obd2_response[7] <= 'F'))) {
        sprintf(hex_buf, "%c%c", obd2_response[6], obd2_response[7]);
        b = strtol(hex_buf, &ptr, 16);
    }

    // printf("  --> [OBD Response] values are: a = %d, b = %d\n", a, b);
    char req_test[16];
    char req_pattern[16];

    // figuring out what could have been the request
    // eg. if OBD2 response is "4104" then the request was "0104", req_test will be compared against dimensions
    sprintf(req_test, "%s", obd2_response);
    if (strlen(req_test) >= 2) {
        req_test[0] = '0';
        req_test[1] = '1';
    }

    // Engine Load
    sprintf(req_pattern, "%s", obd2_request_calculated_engine_load());
    remove_char(req_pattern, ' ');

    if (strncmp(req_test, req_pattern, 4) == 0) {
        app_state.obd2_values.engine_load = ceil(a / 2.55); // result is a number between 0 to 100 (engine load in %)
        app_state.obd2_values.engine_load = ceil(app_state.obd2_values.engine_load / 11.1); // result is a number between 0 and 9 (can be displayed on led strip)

        if (app_state.obd2_values.engine_load > 9) {
            app_state.obd2_values.engine_load = 9;
        }

        led_strip_set(app_state.obd2_values.engine_load);
    }

    // RPM
    sprintf(req_pattern, "%s", obd2_request_engine_rpm());
    remove_char(req_pattern, ' ');

    if (strncmp(req_test, req_pattern, 4) == 0) {
        printf("Detected as RPM value\n");
        int baseline_rpm = 900;
        int max_rpm = 3600;
        double magic = 9 / (double)(max_rpm - baseline_rpm); // 0.00333333333

        app_state.obd2_values.rpm = ((256 * a) + b) / 4; // value from 0 to 16383
        app_state.obd2_values.rpm -= baseline_rpm;
        app_state.obd2_values.rpm = ceil((double)(app_state.obd2_values.rpm * (double)magic));

        if (app_state.obd2_values.rpm > 9) {
            app_state.obd2_values.rpm = 9;
        }
        printf("Setting led strip status to: %d as a=%d, b=%d\n", app_state.obd2_values.rpm, a, b);
        led_strip_set(app_state.obd2_values.rpm);
    } else printf("NOT Detected as RPM value\n");

    // Distance to Empty
    sprintf(req_pattern, "%s", obd2_request_fuel_level());
    remove_char(req_pattern, ' ');

    if (strncmp(req_test, req_pattern, 4) == 0) {
        app_state.obd2_values.fuel_level = (double)(a / (double)2.55); // fuel level in % (value from 0 to 100)
        app_state.obd2_values.fuel_in_liter = (double)(app_state.obd2_values.fuel_level / 100) * FUEL_TANK_LITER;
        app_state.obd2_values.distance_to_empty_km = ((double)app_state.obd2_values.fuel_in_liter / (double)AVERAGE_FUEL_CONSUMPTION_PER_100_KM) * 100;
        printf("  --> Distance to empty set to: %d\n", app_state.obd2_values.distance_to_empty_km);
        refresh_lcd_display();
    }

    // Coolant temp
    sprintf(req_pattern, "%s", obd2_request_engine_coolant_temp());
    remove_char(req_pattern, ' ');

    if (strncmp(req_test, req_pattern, 4) == 0) {
        app_state.obd2_values.coolant_temp_in_celsius = a - 40;
        printf("  --> Collant temp set to: %d\n", app_state.obd2_values.coolant_temp_in_celsius);
        refresh_lcd_display();
    }

    // Battery Voltage
    sprintf(req_pattern, "%s", obd2_request_battery_voltage());
    remove_char(req_pattern, ' ');

    if (strncmp(req_test, req_pattern, 4) == 0) {
        app_state.obd2_values.battery_voltage = (double)((double)(255 * a) + b) / (double)1000;
        printf("  --> Battery Voltage: %f\n", app_state.obd2_values.battery_voltage);
        refresh_lcd_display();
    }
}
