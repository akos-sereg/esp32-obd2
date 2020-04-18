#ifndef __elm_327_h_included__
#define __elm_327_h_included__

extern char *obd2_request_calculated_engine_load();
extern char *obd2_request_engine_rpm();
extern char *obd2_request_engine_coolant_temp();
extern char *obd2_request_fuel_level();
extern char *obd2_request_engine_oil_temp();
extern char *obd2_request_engine_exhaust_flow_rate();

#endif