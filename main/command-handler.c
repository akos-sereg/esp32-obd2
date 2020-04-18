#include "include/command-handler.h"

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
}