#ifndef __app_state_h_included__
#define __app_state_h_included__

typedef struct app_state_t {
  struct obd2_bluetooth {
    int is_connected; // true, if we have bluetooth connection with OBD2 device
    int displaying_connected; // if true, we are displaying the message "Connected to OBD2" on LCD display
    int displaying_connected_elapsed_ms; // number of milliseconds since we are displaying "Connected to OBD2" message
    int displaying_connecting_elapsed_ms; // number of milliseconds since we are displaying "Connecting to OBD" message

    // if true, we already displayed "Connected to OBD2" message on LCD display for enough time,
    // and we should no longer have it displayed.
    int displayed_connected;
  } obd2_bluetooth;

  struct obd2_values {
    int engine_load; // value from 0 to 9
    int distance_to_empty_km;
    double fuel_level; // value from 0% to 100%
    double fuel_in_liter;
    int coolant_temp_in_celsius;
    double battery_voltage;
    int rpm; // value from 0 to 9

  } obd2_values;
} app_state_t;

extern app_state_t app_state;
extern void reset_app_state();

#endif