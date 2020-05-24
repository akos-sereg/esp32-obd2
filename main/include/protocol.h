#ifndef __protocol_h_included__
#define __protocol_h_included__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "time.h"
#include "sys/time.h"
#include "esp_spp_api.h"

#include "bt-common.h"
#include "string-helper.h"
#include "command-handler.h"

#define BT_RESPONSE_DATA_MAXLEN		32
#define BT_REQUEST_DATA_MAXLEN		32

/*
// for live:
#define BT_ENGINE_LOAD_POLL_INTERVAL 200 // in msec
#define BT_RESTART_POLLING_ENGINE_LOAD_AFTER    60000 // in msec - in case OBD2 did not respond for a while, restart polling
#define BT_LCD_DATA_POLLING_INTERVAL 3000 // in msec
*/


// for testing:
#define BT_ENGINE_LOAD_POLL_INTERVAL 200 // in msec
#define BT_RESTART_POLLING_ENGINE_LOAD_AFTER    60000 // in msec - in case OBD2 did not respond for a while, restart polling
#define BT_LCD_DATA_POLLING_INTERVAL 3000 // in msec

extern char bt_response_data[BT_RESPONSE_DATA_MAXLEN];
extern int bt_response_data_len;
int64_t time_last_lcd_data_received;

extern void bt_response_chunk_received(uint8_t *chunk, int length);
extern void bt_send_data(char *);
extern int64_t bt_get_last_request_sent();
extern int64_t get_epoch_milliseconds();
extern int64_t get_time_last_lcd_data_sent();
extern void reset_time_last_lcd_data_sent();
extern void instant_fetch_lcd_data();

#endif