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

## Configuration

- Polling interval of Engine Load (led strip): `BT_ENGINE_LOAD_POLL_INTERVAL` in `include/protocol.h`
- Restart polling Engine Load in case of OBD2 failure: `BT_RESTART_POLLING_ENGINE_LOAD_AFTER` in `include/protocol.h`
- Fuel tank in litres: `FUEL_TANK_LITER` in `include/app_main.h`
- Your OBD2 device's MAC address: `remote_device_addr_1` in `bluetooth.c`
- Your car's average fuel consumption: `AVERAGE_FUEL_CONSUMPTION_PER_100_KM` in `include/app_main.h`
- Led strip to display Engine Load or RPM: `LED_STRIP_DISPLAYS_RPM` in `include/app_main.h`
- Long key press interval (to set backlight on/off) `LONG_KEYPRESS_INTERVAL_MS` in `include/switches.h`
- In case your OBD2 device requires PIN authentication, update this line `memcpy(pin_code, "1234", 4);` in `bluetooth.c`

## How to test

Download `BT SPP Server` android app, and configure esp32 device to use your phone's MAC address when connecting via 
bluetooth. Open BT SPP Server app, and wait until it says "Connected". If esp32 device cannot connect to your server, 
open bluetooth settings (eg. list bluetooth devices in range), then navigate back to the app - this usually helps, most probably 
android starts advertising when you are at bluetooth settings screen.

### Led strip

Assuming that your device is configured to show RPM, just type `410C1234` into BT SPP Server app (eg. send it to the device). 
Device will automatically handle your command as OBD2 response (41) for RPM (0C) with value 0x12 0x34 and it will display it on the led strip.

For Engine Load, use command `41051234` (05 is Engine Load)

## Status

Device can read data from OBD2 via bluetooth, and it can display stuff on LCD, such as "Distance to empty" based on 
fuel level, Engine coolant temperature, Battery voltage, etc. It can also display Engine Load or RPM on the led strip.

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

Get your OBD2 bluetooth device's address (plug into your car, check what device you see on your smartphone). 
Update `main/bluetooth.c`, find `remote_device_addr_1` and set it to your OBD2 device's address. You can set a 
secondary MAC address as well: `remote_device_addr_2`, esp32 device will connect to the one that is available. 
Whenever you are testing in your car, disable your bluetooth to make sure that device will connect to the real OBD2 device, 
not to your phone.

## Build

Options choose step:
    1. idf.py menuconfig.
    2. enter menuconfig "Component config", choose "Bluetooth"
    3. enter menu Bluetooth, choose "Classic Bluetooth" and "SPP Profile"
    4. choose your options.

## TODOs

- Using list of acceptable devices (eg. phone's MAC at home, OBD2 MAC at car), to avoid configuration overhead
- Initialize OBD device, echo off, etc.