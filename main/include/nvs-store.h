#ifndef __nvs_store_h_included__
#define __nvs_store_h_included__

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

extern void init_nvs_store();
extern void set_nvs_value(char *key, int32_t value);
extern int32_t get_nvs_value(char *key);
extern void nvs_shutdown();

nvs_handle_t app_nvs_handle;
esp_err_t nvs_error;
int nvs_flash_is_open;

char *NVS_KEY_MODE;
char *NVS_KEY_LCD_BACKLIGHT;

#endif