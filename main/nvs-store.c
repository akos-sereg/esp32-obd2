#include "include/nvs-store.h"

nvs_handle_t app_nvs_handle;
esp_err_t nvs_error;
int nvs_flash_is_open;

char *NVS_KEY_MODE = "mode";
char *NVS_KEY_LCD_BACKLIGHT = "bl";

void init_nvs_store() {
    nvs_error = nvs_flash_init();
    if (nvs_error == ESP_ERR_NVS_NO_FREE_PAGES || nvs_error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_error = nvs_flash_init();
    }
    ESP_ERROR_CHECK( nvs_error );
    nvs_error = nvs_open("storage", NVS_READWRITE, &app_nvs_handle);

    if (nvs_error != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(nvs_error));
        nvs_flash_is_open = 0;
    } else {
        nvs_flash_is_open = 1;
    }
}

void set_nvs_value(char *key, int32_t value) {
    if (!nvs_flash_is_open) {
        return;
    }

    nvs_error = nvs_set_i32(app_nvs_handle, key, value);
    printf((nvs_error != ESP_OK) ? "Failed to write into NVS store!\n" : "Done writing into NVS store\n");

    if (nvs_error != ESP_OK) {
        printf("Error (%s) writing!\n", esp_err_to_name(nvs_error));
    }

    nvs_error = nvs_commit(app_nvs_handle);
    printf((nvs_error != ESP_OK) ? "Failed to commit NVS store changes!\n" : "NVS commit done\n");
}

int32_t get_nvs_value(char *key) {
    int32_t read_value = -1;
    if (!nvs_flash_is_open) {
        return read_value;
    }

    nvs_error = nvs_get_i32(app_nvs_handle, key, &read_value);
    switch (nvs_error) {
        case ESP_OK:
            return read_value;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value for key '%s' is not initialized yet\n", key);
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(nvs_error));
    }

    return read_value;
}

void nvs_shutdown() {
    nvs_close(app_nvs_handle);
}