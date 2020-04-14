#include "include/lcd.h"
#include "include/switches.h"

void i2c_master_init(void)
{
    // Set up I2C
    printf("Set up I2C\n");
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
    printf("Set up SMBus\n");
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Set up the LCD1602 device with backlight off
    printf("Set up LCD1602 device with backlight off");
    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true);

    // turn on backlight
    i2c_lcd1602_set_backlight(lcd_info, true);
}

void lcd_display_text(char *line1, char *line2) {
    i2c_lcd1602_clear(lcd_info);
    i2c_lcd1602_write_string(lcd_info, line1);

    if (line2 != NULL) {
	i2c_lcd1602_move_cursor(lcd_info, 0, 1);
	i2c_lcd1602_write_string(lcd_info, line2);
    }
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
	    sprintf(line, "%d km", app_state.obd2_values.distanceToEmptyInKm);
	    i2c_lcd1602_clear(lcd_info); // also returns to home
	    i2c_lcd1602_write_string(lcd_info, "Dist. to empty");
	    i2c_lcd1602_move_cursor(lcd_info, 5, 1);
	    i2c_lcd1602_write_string(lcd_info, line);
	    break;

	case 1:
	    i2c_lcd1602_clear(lcd_info); // also returns to home
	    i2c_lcd1602_write_string(lcd_info, "Coolant temp.");
	    i2c_lcd1602_move_cursor(lcd_info, 5, 1);
	    i2c_lcd1602_write_string(lcd_info, "92 C");
	    break;

	case 2:
	    i2c_lcd1602_clear(lcd_info); // also returns to home
	    i2c_lcd1602_write_string(lcd_info, "Engine Load");
	    i2c_lcd1602_move_cursor(lcd_info, 5, 1);
	    i2c_lcd1602_write_string(lcd_info, "67%");
	    break;
	case 3:
	    i2c_lcd1602_set_backlight(lcd_info, false);
	    break;
    }
}