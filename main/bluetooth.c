/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "include/bt_common.h"
#include "include/protocol.h"

#include "time.h"
#include "sys/time.h"

#define SPP_TAG "SPP_INITIATOR_DEMO"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_INITIATOR"

#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static struct timeval time_new, time_old;
static long data_num = 0;
static char buf[1024];

// static const esp_spp_sec_t sec_mask_authenticate = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_sec_t sec_mask_authorize = ESP_SPP_SEC_AUTHORIZE;
static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

static esp_bd_addr_t peer_bd_addr;
static uint8_t peer_bdname_len;
static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
// static const char remote_device_name[] = "CBT.";
// static const char remote_device_addr[] = "00:0d:18:3a:61:fc"; // OBD2 device
static const char remote_device_addr[] = "30:ae:a4:6a:a9:7a"; // Test device

static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;
static const uint8_t inq_len = 30;
static const uint8_t inq_num_rsps = 0;
static int display_num = 0;

#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
#define SPP_DATA_LEN 20
#else
#define SPP_DATA_LEN ESP_SPP_MAX_MTU
#endif
static uint8_t spp_data[SPP_DATA_LEN];

static void print_speed(void)
{
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(SPP_TAG, "speed(%fs ~ %fs): %f kbit/s" , time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}

static bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    //esp_spp_cb_param = param;

    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);

        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT status=%d scn_num=%d",param->disc_comp.status, param->disc_comp.scn_num);
        if (param->disc_comp.status == ESP_SPP_SUCCESS) {
            // esp_spp_connect(sec_mask_authenticate, role_master, param->disc_comp.scn[0], peer_bd_addr);
        }

        printf("Total number of Channels: %d\n", param->disc_comp.scn_num);
        esp_spp_connect(sec_mask_authorize, role_master, param->disc_comp.scn[0], peer_bd_addr);
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
	printf("Writing 'ping 8' to acceptor\n");
        // esp_spp_write(param->srv_open.handle, SPP_DATA_LEN, spp_data);
	bt_handle = param->srv_open.handle;
	bt_is_connected = 1;
	bt_send_data("hello world");
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
	bt_is_connected = 0;
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT");

	// receive
	printf("Received data length: %d\n", param->data_ind.len);

	// clear response data
	for (int i=0; i!=BT_RESPONSE_DATA_MAXLEN; i++) {
	    bt_response_data[i] = 0;
	}

	for (int i=0; i!=param->data_ind.len; i++) {
	    bt_response_data[i] = param->data_ind.data[i];
	}

	/*if (param->data_ind.len < 1023) {

	    for (int i=0; i!=param->data_ind.len; i++) {
		printf(" -> data[%d] = %d => %c\n", i, param->data_ind.data[i], param->data_ind.data[i]);
	    }

	    snprintf(buf, (size_t)param->data_ind.len, (char *)param->data_ind.data);
	    printf(buf);
	    printf("\n");
	    // snprintf(spp_data, "Received characters: %d\n", param->data_ind.len);
	} else {
	    esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
	}*/

        break;
    case ESP_SPP_CONG_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        //ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT cong=%d", param->cong.cong);
#endif
        /*if (param->cong.cong == 0) {
            esp_spp_write(param->cong.handle, SPP_DATA_LEN, spp_data);
        }*/
        break;
    case ESP_SPP_WRITE_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)

	printf("ESP_SPP_WRITE_EVT\n");
	display_num++;
	if (display_num == 100) {
    	    ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT len=%d cong=%d", param->write.len , param->write.cong);
    	    esp_log_buffer_hex("",spp_data,SPP_DATA_LEN);
	    display_num = 0;
	}
#else
        gettimeofday(&time_new, NULL);
        data_num += param->write.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        if (param->write.cong == 0) {
            // esp_spp_write(param->write.handle, SPP_DATA_LEN, spp_data);
        }
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        break;
    default:
        break;
    }
}

/*

et up I2C
Set up SMBus
Set up LCD1602 device with backlight offI (1212) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (1232) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (1232) SPP_INITIATOR_DEMO: 00 0d 18 3a 61 fc
Device found, not sure if name exist
Device found, not sure if name exist
I (2042) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (2042) SPP_INITIATOR_DEMO: 00 0d 18 3a 61 fc
Device found, not sure if name exist
Device found, not sure if name exist

 (1222) gpio: GPIO[34]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:1
Set up I2C
Set up SMBus
Set up LCD1602 device with backlight offI (1232) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (1242) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (1242) SPP_INITIATOR_DEMO: 00 0d 18 3a 61 fc
Device found, not sure if name exist
Device found, not sure if name exist
I (1262) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (1262) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=2 scn_num=0
I (1262) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (1402) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=1 scn_num=0


Set up I2C
Set up SMBus
Set up LCD1602 device with backlight offI (1212) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (9962) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_RES_EVT
I (9962) SPP_INITIATOR_DEMO: 00 0d 18 3a 61 fc
Device found, not sure if name exist, param->disc_res.prop[i].type = 2, eir: 0x3ffcce4c
Device found, not sure if name exist, param->disc_res.prop[i].type = 3, eir: 0x3ffba193
I (9992) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (9992) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=2 scn_num=0
E (9992) BT_LOG: Only ESP_SPP_SEC_AUTHORIZE is supported!

I (10002) SPP_INITIATOR_DEMO: ESP_BT_GAP_DISC_STATE_CHANGED_EVT
I (10272) SPP_INITIATOR_DEMO: ESP_SPP_DISCOVERY_COMP_EVT status=1 scn_num=0
E (10272) BT_LOG: Only ESP_SPP_SEC_AUTHORIZE is supported!




*/

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bt_address[64];

    switch(event){
    case ESP_BT_GAP_DISC_RES_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
        esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);

        sprintf(bt_address, "%02x:%02x:%02x:%02x:%02x:%02x",
          param->disc_res.bda[0],
          param->disc_res.bda[1],
          param->disc_res.bda[2],
          param->disc_res.bda[3],
          param->disc_res.bda[4],
          param->disc_res.bda[5]);

        if (strncmp(remote_device_addr, bt_address, strlen(remote_device_addr)) == 0) {

          printf("OBD2 device found: %s\n", bt_address);

          memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
          esp_spp_start_discovery(peer_bd_addr);
          esp_bt_gap_cancel_discovery();
        } else {
          printf("Device found, but it is not the target device: %s\n", bt_address);
        }

        /*for (int i = 0; i < param->disc_res.num_prop; i++){

            printf("Device found, not sure if name exist, param->disc_res.prop[i].type = %d, eir: %p\n",
              param->disc_res.prop[i].type,
              param->disc_res.prop[i].val);

            memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
            esp_spp_start_discovery(peer_bd_addr);
            esp_bt_gap_cancel_discovery();


            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR
                && get_name_from_eir(param->disc_res.prop[i].val, peer_bdname, &peer_bdname_len)){
                esp_log_buffer_char(SPP_TAG, peer_bdname, peer_bdname_len);
                if (peer_bdname_len > 0) {
                  printf("Device found: %s\n", peer_bdname);
                }
                if (strlen(remote_device_name) == peer_bdname_len
                    && strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                    memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                    esp_spp_start_discovery(peer_bd_addr);
                    esp_bt_gap_cancel_discovery();
                }
            }
        }*/
        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVCS_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVCS_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVC_REC_EVT");
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default:
        break;
    }
}

void init_bluetooth(void)
{
    for (int i = 0; i < SPP_DATA_LEN; ++i) {
        spp_data[i] = 0;
    }

    spp_data[0] = 'p';
    spp_data[1] = 'i';
    spp_data[2] = 'n';
    spp_data[3] = 'g';
    spp_data[4] = ' ';
    spp_data[5] = '8';

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}

