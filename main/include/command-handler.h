#ifndef __command_handler_h_included__
#define __command_handler_h_included__

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "engine-load-leds.h"
#include "lcd.h"

extern void handle_obd2_response(char *obd2_response_chunk, int is_lcd_value_request);

#endif