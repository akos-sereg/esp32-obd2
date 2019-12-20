#ifndef __protocol_h_included__
#define __protocol_h_included__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_spp_api.h"

#define BT_RESPONSE_DATA_MAXLEN		32
#define BT_REQUEST_DATA_MAXLEN		32

extern char bt_response_data[BT_RESPONSE_DATA_MAXLEN];

extern void bt_send_data(char *);

#endif