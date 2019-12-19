# esp32-obd2

OBDII based car diagnostics dashboard.

![Board](https://raw.githubusercontent.com/akos-sereg/esp32-obd2/master/docs/board.jpg)

## Purpose of this project

There is a smartphone holder hole in the center dashboard's lower section in my car, which is 
out of use (my smartphone does not fit there), so I wanted to fill up that unused space with 
a device which reads data from the car's OBDII interface, and displays Engine Load, Coolant temp. 
and other metrics.

## Usage

### Display

- There is a led strip (6 green, 2 yellow, 1 red) which is supposed to display the Engine Load
- LCD display shows more information, based on it'smode set by push button

### Buttons

- Power On: power supply from the car's USB outlet
- Push button: change modes, display different metrics on LCD: coolant temp, distance to empty, etc.

## Status

This is a pilot project, right now it only prints some random stuff, and the Engine Load 
display is also in demo mode. I am testing the display in the car to validate that it's not 
disturbing at night / and visible in daylight. Later on it will pull data from OBD2 interface 
via bluetooth.

## Circuit

### Pinout on shift register's board

This board has no circuit documentation right now. The board is responsible for driving 
8 leds that are supposed to display Engine Load: 6 green, 2 yellow.

- 1-8: Leds
- 16: 3.3V
- 15: GND from ESP32
- 14: CLK
- 13: LATCH
- 12: DATA
- 11: GND for display LEDs

### Pinout on ESP32

### Driving shift register (Engine Load)

- GPIO 32: LATCH
- GPIO 25: DATA
- GPIO 33: CLOCK

### Driving LCD 1602 display

- GPIO 26: DATA
- GPIO 27: CLOCK

### Driving last LED of Engine Load display (red)

- GPIO 12: Red LED

### Switch input (mode selection for LCD display)

- GPIO 34: Mode selection (1.5K Ohm to GND, 1K Ohm to signal)

## ESP-IDF

espressif/esp-idf commit hash 451f69cc2e11cf45e3a72905c9fb162ca9a08936

## Configure

Get your OBD2 bluetooth device's name (plug into your car, check what device you see on your smartphone). 
Update `main/bluetooth.c`, find `device_name` and set it to your OBD2 device name.

## Build

Options choose step:
    1. idf.py menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and "SPP Profile"
    4. choose your options.
