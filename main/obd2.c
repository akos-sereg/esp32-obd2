#include "include/obd2.h"

char *obd2_request_calculated_engine_load() {
    return "01 04";
}

char *obd2_request_engine_rpm() {
    return "01 0C";
}

char *obd2_request_engine_coolant_temp() {
    return "01 05";
}

char *obd2_request_fuel_level() {
    return "01 2F";
}

char *obd2_request_engine_oil_temp() {
    return "01 5C";
}

char *obd2_request_engine_exhaust_flow_rate() {
    return "01 9E";
}
