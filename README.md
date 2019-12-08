# esp32-i2c-lcd1602-example

[![Build Status](https://travis-ci.org/DavidAntliff/esp32-i2c-lcd1602-example.svg?branch=master)](https://travis-ci.org/DavidAntliff/esp32-i2c-lcd1602-example)
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)]()

## Introduction

This is an example application for the HD4470-compatible LCD1602 device connected via an I2C backpack.

It is written and tested for v2.1.1 and v3.0 of the [ESP-IDF](https://github.com/espressif/esp-idf) environment, using the xtensa-esp32-elf toolchain (gcc version 5.2.0).

Ensure that submodules are cloned:

    $ git clone --recursive https://github.com/DavidAntliff/esp32-i2c-lcd1602-example.git

Build the application with:

    $ cd esp32-i2c-lcd1602-example.git
    $ make menuconfig    # set your serial configuration and the I2C GPIO - see below
    $ make flash monitor

The program should detect your connected device and display some demonstration text on the LCD.

## Dependencies

This application makes use of the following components (included as submodules):

 * components/[esp32-smbus](https://github.com/DavidAntliff/esp32-smbus)
 * components/[esp32-i2c-lcd1602](https://github.com/DavidAntliff/esp32-i2c-lcd1602)

## Hardware

To run this example, connect one LCD1602 device to two GPIOs on the ESP32 (I2C SDA and SCL). If external pull-up resistors are not provided with the sensor, add a 10 KOhm resistor from each GPIO to the 3.3V supply.

`make menuconfig` can be used to set the I2C GPIOs and LCD1602 device I2C address.

## Features

This example steps through the features of the esp32-i2c-lcd1602 component. It demonstrates:

 * Display initialisation, disabling and enabling, clearing.
 * Backlight control.
 * Underline and blinking cursor control, including arbitrary cursor movement and homing.
 * Display scrolling.
 * Custom character definition.
 * Display of all characters.

Each step waits for a keypress on stdin before executing. If stdin is not available (no USB/UART available), modify the following code:

```
//#define USE_STDIN  1
#undef USE_STDIN
```

Each step will then wait for one second before proceeding automatically.

## Source Code

The source is available from [GitHub](https://www.github.com/DavidAntliff/esp32-i2c-lcd1602-example).

## License

The code in this project is licensed under the MIT license - see LICENSE for details.

## Links

 * [HD44780 Dot Matrix Liquid Crystal Display Controller/Driver datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)
 * [Espressif IoT Development Framework for ESP32](https://github.com/espressif/esp-idf)

## Acknowledgements

 * "I2C" is a registered trademark of Phillips Corporation.
 * "SMBus" is a trademark of Intel Corporation.
 
