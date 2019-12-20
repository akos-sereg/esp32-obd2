#ifndef __app_state_h_included__
#define __app_state_h_included__

typedef struct app_state_t {
  struct obd2_bluetooth {
    /* if true, we are displaying the message "Connected to OBD2" on LCD display */
    int displaying_connected;

    /* number of milliseconds since we are displaying "Connected to OBD2" message */
    int displaying_connected_elapsed_ms;

    /* number of milliseconds since we are displaying "Connecting to OBD" message */
    int displaying_connecting_elapsed_ms;

    /*
       if true, we already displayed "Connected to OBD2" message on LCD display for enough time,
       and we should no longer have it displayed.
    */
    int displayed_connected;
  } obd2_bluetooth;
} app_state_t;

extern app_state_t app_state;

#endif