#include "include/state.h"

app_state_t app_state;

void reset_app_state() {
    app_state.obd2_values.distance_to_empty_km = 0;
    app_state.obd2_values.engine_load = 0;
}