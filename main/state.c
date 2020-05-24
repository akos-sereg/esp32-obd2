#include "include/state.h"

app_state_t app_state;

void reset_app_state() {
    app_state.obd2_values.distance_to_empty_km = 0;
    app_state.obd2_values.engine_load = 0;
    app_state.obd2_values.coolant_temp_in_celsius = 0;
    app_state.obd2_values.battery_voltage = 0.0;
    app_state.obd2_values.rpm = 0;
    app_state.obd2_values.fuel_level = 0;
    app_state.obd2_values.fuel_in_liter = 0.0;
}