#include "include/obd2.h"

char *obd2_request_calculated_engine_load() { return "01 04"; }
char *obd2_request_engine_rpm() { return "01 0C"; }
char *obd2_request_engine_coolant_temp() { return "01 05"; }
char *obd2_request_fuel_level() { return "01 2F"; }
char *obd2_request_battery_voltage() { return "01 42"; }

void obd2_init_communication() {
    bt_send_data("AT E0"); // echo off
    bt_send_data("AT L0"); // turns off extra line feed and carriage return
    // bt_send_data("AT H0"); // turns off headers and checksum to be sent
    // bt_send_data("ATAT2"); // turn adaptive timing to 2
    // bt_send_data("ATST0A"); // set timeout to 10 * 4 = 40msec, allows +20 queries per second. This is the maximum wait-time. ATAT will decide if it should wait shorter or not
    // bt_send_data("ATSP0"); // set the protocol to automatic

    // clear response data in case we have any response from device
    bt_response_data_len = 0;
}
