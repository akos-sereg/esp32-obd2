#include "include/protocol.h"
#include "include/bt_common.h"

char bt_response_data[BT_RESPONSE_DATA_MAXLEN];
int bt_response_data_len = 0;

void bt_send_data(char *data) {
    uint8_t bt_request_data[BT_REQUEST_DATA_MAXLEN];

    for (int i=0; i!=BT_RESPONSE_DATA_MAXLEN; i++) {
        bt_response_data[i] = 0;
    }

    for (int i=0; i!=BT_REQUEST_DATA_MAXLEN; i++) {
        bt_request_data[i] = 0;
    }

    for (int i=0; i!=strlen(data); i++) {
        bt_request_data[i] = data[i];
    }

    esp_spp_write(bt_handle, strlen(data), bt_request_data);
}
