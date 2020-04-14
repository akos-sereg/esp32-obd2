#include "include/state.h"

app_state_t app_state;

void reset_app_state() {
    app_state.obd2_values.distanceToEmptyInKm = 0;
}