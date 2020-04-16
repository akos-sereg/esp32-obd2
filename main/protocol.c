#include "include/protocol.h"
#include "include/bt_common.h"

char bt_response_data[BT_RESPONSE_DATA_MAXLEN];
int bt_response_data_len = 0;
int bt_response_processed = 1; // pretend that our starting point is that we processed any data
int64_t bt_last_request_sent = 0; // epoch in milliseconds
int64_t time_last_lcd_data_received = -20000; // epoch in milliseconds, pretend that it has been soo long when we received data
int bt_waiting_for_response = 0;

void bt_send_data(char *data) {
    uint8_t bt_request_data[BT_REQUEST_DATA_MAXLEN];
    esp_err_t bt_error;

    for (int i=0; i!=BT_RESPONSE_DATA_MAXLEN; i++) {
        bt_response_data[i] = 0;
    }

    for (int i=0; i!=BT_REQUEST_DATA_MAXLEN; i++) {
        bt_request_data[i] = 0;
    }

    for (int i=0; i!=strlen(data); i++) {
        bt_request_data[i] = data[i];
    }

    printf("Sending data to OBD2: %s", data);
    bt_error = esp_spp_write(bt_handle, strlen(data), bt_request_data);

    if (bt_error != ESP_OK) {
        printf("ERROR: %s\n", esp_err_to_name(bt_error));
    } else {
        bt_last_request_sent = get_epoch_milliseconds();
        bt_response_processed = 0;
        bt_response_data_len = 0;
        bt_waiting_for_response = 1;
    }
}

int64_t bt_get_last_request_sent() {
    return bt_last_request_sent;
}

int64_t get_time_last_lcd_data_received() {
    return time_last_lcd_data_received;
}

void reset_time_last_lcd_data_received() {
    time_last_lcd_data_received = get_epoch_milliseconds();
}


// todo: move this to a helper
int64_t get_epoch_milliseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}