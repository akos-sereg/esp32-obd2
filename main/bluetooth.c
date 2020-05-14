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
#include "time.h"
#include "sys/time.h"

#include "include/bluetooth.h"

#define SPP_TAG "SPP_INITIATOR_DEMO"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_INITIATOR"

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static const esp_spp_sec_t sec_mask_authorize = ESP_SPP_SEC_AUTHORIZE;
static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

static esp_bd_addr_t peer_bd_addr;
static const char remote_device_addr_1[] = "00:0d:18:3a:61:fc"; // OBD2 device
static const char remote_device_addr_2[] = "3c:05:18:7c:76:3d"; // Samsung J5

uint8_t remote_bda;
static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;
static const uint8_t inq_len = 30;
static const uint8_t inq_num_rsps = 0;

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
        printf("Total number of Channels: %d\n", param->disc_comp.scn_num);
        remote_bda = peer_bd_addr;

        if (param->disc_comp.status == ESP_SPP_SUCCESS) {
            // this path is executed in case you are connecting to "BT SPP Server" Android app
            esp_spp_connect(sec_mask_authorize, role_master, param->disc_comp.scn[0], peer_bd_addr);
        } else {
            // OBD2 device found, but SCNs are not available
            // connecting to OBD2 device with scn = 1 (dont ask what that "1" means)
            esp_spp_connect(sec_mask_authorize, role_master, 1, peer_bd_addr);
        }

        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");

        bt_handle = param->srv_open.handle;

        obd2_init_communication();

        app_state.obd2_bluetooth.is_connected = 1;

        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
	app_state.obd2_bluetooth.is_connected = 0;
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:

        bt_response_chunk_received(param->data_ind.data, param->data_ind.len);

        break;
    case ESP_SPP_CONG_EVT:
        break;
    case ESP_SPP_WRITE_EVT:
        printf("ESP_SPP_WRITE_EVT\n");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        break;
    default:
        break;
    }
}

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

        if (strncmp(remote_device_addr_1, bt_address, strlen(remote_device_addr_1)) == 0
            || strncmp(remote_device_addr_2, bt_address, strlen(remote_device_addr_2)) == 0) {

          printf("OBD2 device found: %s\n", bt_address);

          memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
          esp_spp_start_discovery(peer_bd_addr);
          esp_bt_gap_cancel_discovery();
        } else {
          printf("Device found, but it is not the target device: %s\n", bt_address);
        }

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
            // when remote device completes authentication
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
            memcpy(pin_code, "1234", 4);
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        // when remote device tries to connect
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
    memcpy(pin_code, "1234", 4);
    esp_bt_gap_set_pin(pin_type, 4, pin_code);
#if (CONFIG_BT_SSP_ENABLED == true)
    printf("BT SSP Enabled");
#else
    printf("BT SSP Disabled");
#endif
}

