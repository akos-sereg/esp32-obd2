#ifndef __command_handler_h_included__
#define __command_handler_h_included__

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "led-strip.h"
#include "lcd.h"
#include "app_main.h"

extern void handle_obd2_response(char *obd2_response_chunk);

#endif