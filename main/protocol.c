#include "include/protocol.h"

char bt_response_data[BT_RESPONSE_DATA_MAXLEN];
char bt_response_chunk[BT_RESPONSE_DATA_MAXLEN];
int bt_response_data_len = 0; // consumers will know that obd2 response is available if this value is greater than 0
int bt_response_chunk_len = 0;
int64_t bt_last_request_sent = 0; // epoch in milliseconds
int64_t time_last_lcd_data_sent = 0;

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

    printf("Sending data to OBD2: %s\n", data);
    bt_request_data[strlen(data)] = '\r';
    bt_request_data[strlen(data)+1] = '\n';

    bt_error = esp_spp_write(bt_handle, strlen(data) + 2, bt_request_data);

    if (bt_error != ESP_OK) {
        printf("ERROR: %s\n", esp_err_to_name(bt_error));
    } else {
        bt_last_request_sent = get_epoch_milliseconds();
        bt_response_data_len = 0;
    }
}

// bt_response_data: populated by chunks (eg. multiple calls of this function_
// bt_response_data_len: set when last chunk received (eg. EOL sent from OBD2 device)
//  -> this indicates that response can be processed
// bt_response_chunk: temporary buffer for chunks - appending this buffer for each call of this method
// bt_response_chunk_len: current size of chunk
void bt_response_chunk_received(uint8_t *obd2_response_chunk, int length) {

    for (int i=0; i!=length; i++) {
        bt_response_chunk[bt_response_chunk_len] = obd2_response_chunk[i];
        bt_response_chunk_len++;
    }

    // checking if we have \r\n at the end of the chunk: this means that the response is over
    // and we can process the OBD response
    if ((bt_response_chunk_len >= 2
        && bt_response_chunk[bt_response_chunk_len-2] == '\r'
        && bt_response_chunk[bt_response_chunk_len-1] == '\n')
        || (bt_response_chunk_len >= 1 && bt_response_chunk[bt_response_chunk_len-1] == '>')) {


        // populate response data
        for (int i=0; i!=bt_response_chunk_len; i++) {
            bt_response_data[i] = bt_response_chunk[i];
        }

        bt_response_data[bt_response_chunk_len] = '\0';
        remove_char(bt_response_data, '\n');
        remove_char(bt_response_data, '\r');
        remove_char(bt_response_data, ' ');
        bt_response_data_len = strlen(bt_response_data); // consumers rely on this field - if response data length is set, it will be processed soon

        // clear chunk
        bt_response_chunk_len = 0;

        printf("[OBD Response] last chunk received, payload is '%s'\n", bt_response_data);

        // handle "NO DATA" response
        // in case of this message, we have to make sure that this message goes through the response processing mechanism, to
        // make sure that the next message (send event) will be triggered
        if (bt_response_data_len >= 7
            && bt_response_data[0] == 'N' && bt_response_data[1] == 'O'
            && bt_response_data[3] == 'D' && bt_response_data[4] == 'A' && bt_response_data[5] == 'T' && bt_response_data[6] == 'A') {
            printf(" -> NO DATA response detected\n");
            handle_obd2_response(bt_response_data);
            return;
        }

        // handle echo - response payload should not be processed
        /*if (bt_response_data_len >= 2 && bt_response_data[0] == '0' && bt_response_data[1] == '1') {
            printf(" -> ignoring echo response\n");
            bt_response_data_len = 0; // this is safe, communication will not get stuck because we will receive transmission after this point
            bt_waiting_for_response = 1;
            return;
        }

        // handle ">" response - response payload should not be processed
        if (bt_response_data_len >= 1 && bt_response_data[0] == '>') {
            printf(" -> ignoring command prompt response\n");
            bt_response_data_len = 0;
            bt_waiting_for_response = 1;
            return;
        }*/

        // handle other scenarios, when response does not seem to be a valid OBD2 response
        // usually ECHO messages or command prompts like this: ">"
        if (bt_response_data_len >= 2 && (bt_response_data[0] != '4' || bt_response_data[1] != '1')) {
            printf(" -> ignoring response, looks like this is not a response value, not starting with 41\n");
            bt_response_data_len = 0;
            return;
        }

        // make sure that data will be processed
        handle_obd2_response(bt_response_data);
    }
}

int64_t bt_get_last_request_sent() {
    return bt_last_request_sent;
}

int64_t get_time_last_lcd_data_sent() {
    return time_last_lcd_data_sent;
}

void reset_time_last_lcd_data_sent() {
    time_last_lcd_data_sent = get_epoch_milliseconds();
}

void instant_fetch_lcd_data() {
    time_last_lcd_data_sent = 0;
}


// todo: move this to a helper
int64_t get_epoch_milliseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}