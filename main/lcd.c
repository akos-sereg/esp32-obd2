#include "include/lcd.h"

bool lcd_backlight = true;

/*
   Adding / modifying LCD display pages:
   - refresh_lcd_display() to display the text and value
   - refresh_lcd_display() to know the max number of pages
   - get_lcd_page_obd_code() to respond the correct OBD PID
   - command-handler.c - handle_obd2_response() to convert response based on LCD_DISPLAY_MODE
   - switches.c - MAX_LCD_DISPLAY_MODE to be set (refactor and use this value in refresh_lcd_display)
*/
void i2c_master_init(void)
{
    // Set up I2C
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Set up the LCD1602 device
    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true);

    // turn on backlight
    int32_t backlight_on = get_nvs_value(NVS_KEY_LCD_BACKLIGHT);
    lcd_backlight = (backlight_on == -1 || backlight_on == 1) ? true : false;
    i2c_lcd1602_set_backlight(lcd_info, lcd_backlight);
}

void lcd_display_text(char *line1, char *line2) {
    i2c_lcd1602_clear(lcd_info);
    i2c_lcd1602_write_string(lcd_info, line1);

    if (line2 != NULL) {
        i2c_lcd1602_move_cursor(lcd_info, 0, 1);
        i2c_lcd1602_write_string(lcd_info, line2);
    }
}

void toggle_lcd_backlight() {
    lcd_backlight = !lcd_backlight;
    i2c_lcd1602_set_backlight(lcd_info, lcd_backlight);
    set_nvs_value(NVS_KEY_LCD_BACKLIGHT, lcd_backlight ? 1 : 0);
}

void refresh_lcd_display() {
    char line[32];
    i2c_lcd1602_set_backlight(lcd_info, true);

    if (LCD_DISPLAY_MODE < 0) {
        LCD_DISPLAY_MODE = 0;
    }

    if (LCD_DISPLAY_MODE > 3) {
        LCD_DISPLAY_MODE = 3;
    }

    switch (LCD_DISPLAY_MODE) {
        case 0:
            sprintf(line, "%d km", app_state.obd2_values.distance_to_empty_km);
            i2c_lcd1602_clear(lcd_info);
            i2c_lcd1602_write_string(lcd_info, "Dist. to empty");
            i2c_lcd1602_move_cursor(lcd_info, 5, 1);
            i2c_lcd1602_write_string(lcd_info, line);
            break;

        case 1:
            sprintf(line, "%d %cC", app_state.obd2_values.coolant_temp_in_celsius, 223);
            i2c_lcd1602_clear(lcd_info);
            i2c_lcd1602_write_string(lcd_info, "Engine Coolant");
            i2c_lcd1602_move_cursor(lcd_info, 5, 1);
            i2c_lcd1602_write_string(lcd_info, line);
            break;

        case 2:
            /*if (app_state.obd2_values.engine_oil_temp_in_celsius > 125) {
                sprintf(line, "%d %cC (High)", app_state.obd2_values.engine_oil_temp_in_celsius, 223);
            } else if (app_state.obd2_values.engine_oil_temp_in_celsius > 110) {
                sprintf(line, "%d %cC (OK)", app_state.obd2_values.engine_oil_temp_in_celsius, 223);
            } else {
                sprintf(line, "%d %cC (Warm-up)", app_state.obd2_values.engine_oil_temp_in_celsius, 223);
            }

            i2c_lcd1602_clear(lcd_info);
            i2c_lcd1602_write_string(lcd_info, "Engine Oil");
            i2c_lcd1602_move_cursor(lcd_info, 0, 1);
            i2c_lcd1602_write_string(lcd_info, line);*/

            if (app_state.obd2_values.fuel_pressure < 210) {
                sprintf(line, "%d kPa (Low)", app_state.obd2_values.fuel_pressure);
            } else if (app_state.obd2_values.fuel_pressure < 410) {
                sprintf(line, "%d kPa (OK)", app_state.obd2_values.fuel_pressure);
            } else {
                sprintf(line, "%d kPa (High)", app_state.obd2_values.fuel_pressure);
            }

            i2c_lcd1602_clear(lcd_info);
            i2c_lcd1602_write_string(lcd_info, "Fuel Pressure");
            i2c_lcd1602_move_cursor(lcd_info, 0, 1);
            i2c_lcd1602_write_string(lcd_info, line);
            break;
        case 3:
            // based on http://popupbackpacker.com/wp-content/uploads/2013/12/State-of-Charge-Chart-Typical-Internet.jpg

            if (app_state.obd2_values.battery_voltage > 12.6) {
                sprintf(line, "%.1f V (100%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 12.5) {
                sprintf(line, "%.1f V (90%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 12.42) {
                sprintf(line, "%.1f V (80%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 12.32) {
                sprintf(line, "%.1f V (70%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 12.20) {
                sprintf(line, "%.1f V (60%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 12.06) {
                sprintf(line, "%.1f V (50%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 11.9) {
                sprintf(line, "%.1f V (40%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 11.75) {
                sprintf(line, "%.1f V (30%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 11.58) {
                sprintf(line, "%.1f V (20%%)", app_state.obd2_values.battery_voltage);
            } else if (app_state.obd2_values.battery_voltage > 11.31) {
                sprintf(line, "%.1f V (10%%)", app_state.obd2_values.battery_voltage);
            } else {
                sprintf(line, "%.1f V (0%%)", app_state.obd2_values.battery_voltage);
            }

            i2c_lcd1602_clear(lcd_info);
            i2c_lcd1602_write_string(lcd_info, "Battery");
            i2c_lcd1602_move_cursor(lcd_info, 0, 1);
            i2c_lcd1602_write_string(lcd_info, line);
            // i2c_lcd1602_set_backlight(lcd_info, false);
            break;
    }
}

char *get_lcd_page_obd_code() {
    switch(LCD_DISPLAY_MODE) {
        case 0:
            // fetching fuel level, this is required to calculate "Distance to empty"
            return obd2_request_fuel_level();

        case 1:
            return obd2_request_engine_coolant_temp();

        case 2:
            return obd2_request_fuel_pressure();

        case 3:
            return obd2_request_battery_voltage();
    }

    return NULL;
}