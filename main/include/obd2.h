#ifndef __elm_327_h_included__
#define __elm_327_h_included__

#include "bluetooth.h"

extern char *obd2_request_calculated_engine_load();
extern char *obd2_request_engine_rpm();
extern char *obd2_request_engine_coolant_temp();
extern char *obd2_request_fuel_level();
extern char *obd2_request_battery_voltage();

extern void obd2_init_communication();

#endif