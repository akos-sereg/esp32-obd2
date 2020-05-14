#ifndef __lcd_h_included__
#define __lcd_h_included__

#include "smbus.h"
#include "i2c-lcd1602.h"
#include "state.h"
#include "obd2.h"
#include "switches.h"

#define I2C_MASTER_NUM			I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN		0 		// disabled
#define I2C_MASTER_RX_BUF_LEN		0		// disabled
#define I2C_MASTER_FREQ_HZ		100000
#define I2C_MASTER_SDA_IO		26
#define I2C_MASTER_SCL_IO		27

extern void i2c_master_init(void);
extern void refresh_lcd_display();
extern void lcd_display_text(char *line1, char *line2);
extern char *get_lcd_page_obd_code();
extern void toggle_lcd_backlight();

i2c_lcd1602_info_t *lcd_info;

#endif