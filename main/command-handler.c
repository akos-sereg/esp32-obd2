#include "include/command-handler.h"

/**
 * By nature, OBD2 returns chunks of data.
 *
 */
void handle_obd2_response(char *obd2_response, int is_lcd_value_request) {
    // sample responses:
    //
    // Request  > 01 04                - Engine Load
    // Response > 41 04 3E
    //
    // Request  > 01 2F                - Fuel Level
    // Response > 41 2F DE

    int a = -1; // first byte of response
    int b = -1; // second byte of response
    char *ptr;
    char hex_buf[3];
    double fuel_level;
    int fuel_in_liter;

    if (strlen(obd2_response) >= 8
        && ((obd2_response[6] >= '0' && obd2_response[6] <= '9') || (obd2_response[6] >= 'A' && obd2_response[6] <= 'F'))
        && ((obd2_response[7] >= '0' && obd2_response[7] <= '9') || (obd2_response[7] >= 'A' && obd2_response[7] <= 'F'))) {
        sprintf(hex_buf, "%c%c", obd2_response[6], obd2_response[7]);
        a = strtol(hex_buf, &ptr, 16);
    }

    if (strlen(obd2_response) >= 11
        && ((obd2_response[9] >= '0' && obd2_response[9] <= '9') || (obd2_response[9] >= 'A' && obd2_response[9] <= 'F'))
        && ((obd2_response[10] >= '0' && obd2_response[10] <= '9') || (obd2_response[10] >= 'A' && obd2_response[10] <= 'F'))) {
        sprintf(hex_buf, "%c%c", obd2_response[9], obd2_response[10]);
        b = strtol(hex_buf, &ptr, 16);
    }

    printf("[OBD Response] values are: a = %d, b = %d\n", a, b);

    if (!is_lcd_value_request) {
        // Engine Load
        app_state.obd2_values.engine_load = ceil(a / 2.55); // result is a number between 0 to 100 (engine load in %)
        app_state.obd2_values.engine_load = ceil(app_state.obd2_values.engine_load / 11.1); // result is a number between 0 and 9 (can be displayed on led strip)
        if (app_state.obd2_values.engine_load > 9) {
            app_state.obd2_values.engine_load = 9;
        }

        printf("Engine Load: %d\n", app_state.obd2_values.engine_load);
        engine_load_set(app_state.obd2_values.engine_load);
    } else {
        switch (LCD_DISPLAY_MODE) {
            case 0:
                // calculate "distance to empty"
                fuel_level = a / 2.55; // fuel level in % (value from 0 to 100)
                printf("Fuel level: %f percentage\n", fuel_level);
                fuel_in_liter = ceil((fuel_level / 100) * FUEL_TANK_LITER);
                printf("Fuel in liters: %d\n", fuel_in_liter);

                app_state.obd2_values.distance_to_empty_km = ceil((double)(fuel_in_liter / AVERAGE_FUEL_CONSUMPTION_PER_100_KM) * 100);
                printf("Distance to emtpy: %d\n", app_state.obd2_values.distance_to_empty_km);
                refresh_lcd_display();
                break;
        }
    }

}
/**
 * Command Examples:
 *
 * Engine Load Indicator
 *   "0": load indicator blank
 *   "6": load indicator: 6 leds are ON
 *   "9": load indicator: all leds are ON
 *
 * LCD
 *   "dst 236": lcd displays "Dist. to empty: 236 km"
 */
 /*
void handle_command(char *command, int is_lcd_value_request) {
    char value[64];

    printf("Command received: %s, LCD value: %d\n", command, is_lcd_value_request);

    // handle OBD2 response of "01 04" requests: Engine Load
    if (!is_lcd_value_request) {
        if (strcmp(command, "0") == 0) { engine_load_set(0); return; }
        if (strcmp(command, "1") == 0) { engine_load_set(1); return; }
        if (strcmp(command, "2") == 0) { engine_load_set(2); return; }
        if (strcmp(command, "3") == 0) { engine_load_set(3); return; }
        if (strcmp(command, "4") == 0) { engine_load_set(4); return; }
        if (strcmp(command, "5") == 0) { engine_load_set(5); return; }
        if (strcmp(command, "6") == 0) { engine_load_set(6); return; }
        if (strcmp(command, "7") == 0) { engine_load_set(7); return; }
        if (strcmp(command, "8") == 0) { engine_load_set(8); return; }
        if (strcmp(command, "9") == 0) { engine_load_set(9); return; }
    }
    // handle OBD2 responses based on current LCD page selection
    else {

        // switch (LCD_DISPLAY_MODE) { ... }

        // command is in form "<cmd> <value>", extracting value ...
        char *found = strstr(command, "dst");

        if (found != NULL) {
            if (strlen(command) >= 3 && command[0] == 'd' && command[1] == 's' && command[2] == 't')
            for (int i=0; i!=strlen(command); i++) {
                if (i >= 4) {
                    value[i-4] = command[i];
                }
            }

            value[strlen(command)-4] = '\0';
            printf("Command value: '%s'\n", value);

            if (strncmp(command, "dst", 3) == 0) {
                app_state.obd2_values.distanceToEmptyInKm = atoi(value);
                refresh_lcd_display();
            }
        }
    }
}*/