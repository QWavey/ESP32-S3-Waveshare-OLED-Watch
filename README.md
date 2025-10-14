

# ESP32-S3 AMOLED 2.06" Display Library

A easier to use library for controlling the ESP32-S3 AMOLED 2.06" display with FT3168 touch controller and LVGL integration.


# Arduino LVGL Touch & Display Library

This repository provides two C++ classes for Arduino/ESP32 projects:

* **`TouchClass`** – Handles capacitive touch input via the FT3168 controller over I2C.
* **`ScreenClass`** – Handles display output using the Arduino GFX & DriveBus libraries, integrated with **LVGL** (Light and Versatile Graphics Library).

These classes allow seamless integration of touchscreen displays with LVGL for creating interactive GUI applications.

---

## Features

### TouchClass

* Initializes the FT3168 touch controller via I2C.
* Supports automatic detection of the touch controller address.
* Integrates with LVGL as an input device.
* Provides the following functions:

  * `isTouched()` – Quickly check if the screen is currently touched.
  * `getTouchPoint(x, y)` – Retrieve raw touch coordinates.

### ScreenClass

* Initializes your display hardware using **Arduino GFX + DriveBus**.
* Integrates with LVGL for GUI rendering.
* Provides helper functions for common UI elements:

  * `button_create()` – Create LVGL buttons.
  * `slider_create()` – Create LVGL sliders.
  * `checkbox_create()` – Create LVGL checkboxes.
* Supports turning the display **on/off** while keeping LVGL initialized.

---



## Requirements

* **Arduino IDE** or **PlatformIO**
### Required Dependencies

| Demo                        | Basic Description                                                                                                                                             | Dependency Library      |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------- |
| 01_HelloWorld               | Demonstrates the basic graphics library functions and can also be used to test the basic performance of display screens and the display effect of random text | GFX_Library_for_Arduino |
| 02_GFX_AsciiTable           | Prints ASCII characters in rows and columns on the display screen according to the screen size                                                                | GFX_Library_for_Arduino |
| 03_LVGL_PCF85063_simpleTime | LVGL library displays the current time                                                                                                                        | LVGL, SensorLib         |
| 04_LVGL_QMI8658_ui          | LVGL draws acceleration line chart                                                                                                                            | LVGL, SensorLib         |
| 05_LVGL_AXP2101_ADC_Data    | LVGL displays PMIC data                                                                                                                                       | LVGL, XPowersLib        |
| 06_LVGL_Arduino_v9          | LVGL demonstration                                                                                                                                            | LVGL, Arduino_DriveBus  |
| 07_LVGL_SD_Test             | LVGL displays the contents of TF card files                                                                                                                   | LVGL                    |
| 08_ES8311                   | ES8311 driver demo, playing simple audio                                                                                                                      | —                       |

**Important:** You also need to create a `pin_config.h` file in the same directory with your pin definitions.

## LINKS

- **ARDUINO LIBS DOWNLOAD:** https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.06/tree/main/examples/Arduino-v3.2.0/libraries
- **ESP-IDF LIBS DOWNLOAD:** https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.06/tree/main/examples/ESP-IDF-v5.4.2
- **WIKI:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-2.06




## Installation

1. Download or clone this repository
2. Copy the files to your Arduino libraries directory:
   - Windows: `Documents\Arduino\libraries\ESP_TOUCH_DISPLAY\`
   - Mac: `~/Documents/Arduino/libraries/ESP_TOUCH_DISPLAY/`
   - Linux: `~/Arduino/libraries/ESP_TOUCH_DISPLAY/`
3. Restart Arduino IDE
4. Install dependencies (see table above

### Compilation errors
- Ensure you're using LVGL v9.x
- Update all dependency libraries to latest versions
- Check that global instances are declared properly

## License

This library is provided as-is for use with the ESP32-S3 AMOLED 2.06" display.

## Contributing

Contributions are welcome! Please submit issues and pull requests on the project reposito
