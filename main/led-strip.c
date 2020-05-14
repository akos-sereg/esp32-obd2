/*
    Engine load leds:
    - 6 green
    - 2 yellow
    - 1 red

    The first 8 leds are connected to a shift register, therefore they can be
    driven using 3 GPIOs (latch, clock, data). The 9th led (red) is connected
    to an ESP32 output pin.
    This was a design mistake, I was calculating with 8 leds, but drilled and
    added one more led to the physical display. Whatever.
*/
#include "include/led-strip.h"

void engine_load_init() {
    // init shift register gpios
    gpio_set_direction(LED_STRIP_LATCH_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_CLOCK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_DATA_GPIO, GPIO_MODE_OUTPUT);

    // init red led' gpio
    gpio_set_direction(LED_STRIP_RED, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_STRIP_RED);
}

/* value from 0 to 9 */
void led_strip_set(int value) {

    if (value < 0 || value > 9) {
        return;
    }

    for (int i=0; i!=8; i++) {
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 0);
        gpio_set_level(LED_STRIP_DATA_GPIO, (7 - value) < i ? 1 : 0);
	    gpio_set_level(LED_STRIP_CLOCK_GPIO, 1);
    }

    gpio_set_level(LED_STRIP_LATCH_GPIO, 1);
    gpio_set_level(LED_STRIP_LATCH_GPIO, 0);

    // red led
    gpio_set_level(LED_STRIP_RED, value == 9 ? 1 : 0);
}

void init_animation() {
    int i = 0;
    int j = 0;

    for (j=0; j!=3; j++) {
        for (i=0; i!=10; i++) {
            led_strip_set(i);
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        for (i=9; i!=-1; i--) {
            led_strip_set(i);
            vTaskDelay(50 / portTICK_RATE_MS);
        }
    }

    led_strip_set(0);
}