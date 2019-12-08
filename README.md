# esp32-obd2

OBDII based car diagnostics dashboard.

# Status

This is a pilot project, right now it only prints some random stuff, and the Engine Load 
display is also in demo mode. I am testing the display in the car to validate that it's not 
disturbing at night / and visible in daylight. Later on it will pull data from OBD2 interface 
via bluetooth.

# Circuit

## Pinout on shift register's board

This board has no circuit documentation right now. The board is responsible for driving 
8 leds that are supposed to display Engine Load: 6 green, 2 yellow.

1-8: Leds
16: 3.3V
15: GND from ESP32
14: CLK
13: LATCH
12: DATA
11: GND for display LEDs

## Pinout on ESP32

### Driving shift register (Engine Load)

GPIO 32: LATCH
GPIO 25: DATA
GPIO 33: CLOCK

### Driving LCD 1602 display

GPIO 26: DATA
GPIO 27: CLOCK

### Driving last LED of Engine Load display (red)

GPIO 12: Red LED

### Switch input (mode selection for LCD display)

GPIO 34: Mode selection (1.5K Ohm to GND, 1K Ohm to signal)