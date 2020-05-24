#include "include/switches.h"

int SWITCH_1_STATE = -1;
int MAX_LCD_DISPLAY_MODE = 2; /* max value of LCD_DISPLAY_MODE (3 screens => value should be 2) */
int LCD_DISPLAY_MODE = 0;
int64_t sw_key_pressed_at;
int64_t sw_key_released_at;

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void listen_switches(void* arg)
{
    uint32_t io_num;
    int current_state;
    int pressed_elapsed;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            vTaskDelay(50 / portTICK_RATE_MS);

            current_state = gpio_get_level(io_num);

            // check if button has been pressed for long: lcd backlight should be toggled or  not
            if (sw_key_pressed_at > 0) {
                pressed_elapsed = get_epoch_milliseconds() - sw_key_pressed_at;
                if (pressed_elapsed >= LONG_KEYPRESS_INTERVAL_MS) {
                    toggle_lcd_backlight();
                    sw_key_pressed_at = 0;
                }
            }

            if (io_num == GPIO_INPUT_IO && SWITCH_1_STATE != current_state) {
                printf("GPIO[%d] state: %d\n", io_num, current_state);
                SWITCH_1_STATE = current_state;

                // rising edge
                if (current_state == 1) {
                    sw_key_pressed_at = get_epoch_milliseconds();
                }

                // falling edge
                if (current_state == 0) {


                    // short key press: navigate to next screen
                    // sw_key_pressed_at would be set to 0 already if it was a long key press
                    if (sw_key_pressed_at > 0) {
                        LCD_DISPLAY_MODE++;
                        if (LCD_DISPLAY_MODE == (MAX_LCD_DISPLAY_MODE + 1)) {
                            LCD_DISPLAY_MODE = 0;
                        }

                        set_nvs_value(NVS_KEY_MODE, LCD_DISPLAY_MODE);

                        // we dont want to wait until the next lcd data refresh interval to get the screen updated
                        instant_fetch_lcd_data();
                        refresh_lcd_display();
                    }

                    sw_key_pressed_at = 0;
                }
            }
        }
    }
}

void setup_switches()
{
    gpio_config_t io_conf;

    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;

    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;

    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;

    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;

    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;

    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //start gpio task
    xTaskCreate(listen_switches, "listen_switches", 8192 * 5, NULL, 5, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);

    //remove isr handler for gpio number.
    gpio_isr_handler_remove(GPIO_INPUT_IO);

    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);
}

