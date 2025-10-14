NOTE: CODE IS BROKEN RIGHT NOW, NEEDS FIXES; I WILL TRY TO FIX IT!

# ESP32-S3 AMOLED 2.06" Display Library

A easier to use library for controlling the ESP32-S3 AMOLED 2.06" display with FT3168 touch controller and LVGL integration.

## Installation

1. Download or clone this repository
2. Copy the files to your Arduino libraries directory:
   - Windows: `Documents\Arduino\libraries\ESP_TOUCH_DISPLAY\`
   - Mac: `~/Documents/Arduino/libraries/ESP_TOUCH_DISPLAY/`
   - Linux: `~/Arduino/libraries/ESP_TOUCH_DISPLAY/`
3. Restart Arduino IDE
4. Install dependencies (see table below)

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

## Basic Usage

### Screen Initialization

```cpp
#include "ESP32-S3-Screen-AMOLED-2.06.h"
#include "ESP32-S3-Touch-AMOLED-2.06.h"

// Create global instances
ScreenClass Screen;
TouchClass Touch;

void setup() {
    Serial.begin(115200);
    
    // Initialize LVGL first
    lv_init();
    
    // Initialize screen (optional double buffering)
    if (!Screen.on(false)) {  // false = single buffer, true = double buffer
        Serial.println("Screen initialization failed");
        while(1) delay(1000);
    }
    
    // Initialize touch with 10 second timeout
    if (!Touch.on(10000)) {
        Serial.println("Touch initialization failed");
        while(1) delay(1000);
    }
    
    // Setup LVGL touch input
    lv_indev_t *touch_indev = Touch.setupLVGLInput();
    if (!touch_indev) {
        Serial.println("Failed to setup touch input");
        while(1) delay(1000);
    }
}

void loop() {
    lv_timer_handler();  // LVGL v9.x uses lv_timer_handler() instead of lv_task_handler()
    delay(5);
}
```

### Screen Operations

#### Get Display Information

```cpp
uint32_t width = Screen.width();
uint32_t height = Screen.height();
bool ready = Screen.isInitialized();
Arduino_GFX* gfx = Screen.gfx();  // Note: Changed from getGFX() to gfx()
```

#### Fill Screen

```cpp
Screen.fillScreen(RGB565_BLACK);
Screen.fillScreen(RGB565_WHITE);
```

#### Power Management

```cpp
Screen.sleep();    // Turn off display
Screen.wake();     // Turn on display
Screen.off();      // Complete shutdown
```

#### Brightness Control

```cpp
Screen.setBrightness(128);  // 0-255
uint8_t level = Screen.getBrightness();
```

### Creating UI Elements

#### Basic Button

```cpp
void button_callback(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Serial.println("Button clicked");
    }
}

lv_obj_t *btn = Screen.createButton(
    "Click Me",              // Text
    LV_ALIGN_CENTER,         // Alignment
    0, 0,                    // X, Y offset
    120, 50,                 // Width, Height
    button_callback,         // Callback function
    nullptr                  // Optional user data
);
```

#### Styled Button

```cpp
lv_obj_t *btn = Screen.createStyledButton(
    "Styled",
    LV_ALIGN_CENTER,
    0, 0,
    120, 50,
    button_callback,
    lv_color_hex(0x2196F3),  // Background color
    lv_color_hex(0xFFFFFF),  // Text color
    nullptr                  // Optional user data
);
```

#### Update Button Text

```cpp
Screen.setButtonText(btn, "New Text");
```

#### Create Label

```cpp
lv_obj_t *label = Screen.createLabel(
    "Hello World",
    LV_ALIGN_TOP_MID,
    0, 20,
    nullptr  // Optional parent object
);
```

#### Create Slider

```cpp
void slider_callback(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    Serial.printf("Slider value: %d\n", value);
}

lv_obj_t *slider = Screen.createSlider(
    LV_ALIGN_CENTER,
    0, 50,
    200,        // Width
    0, 100,     // Min, Max
    50,         // Start value
    slider_callback,
    nullptr     // Optional user data
);
```

#### Create Progress Bar

```cpp
lv_obj_t *bar = Screen.createProgressBar(
    LV_ALIGN_CENTER,
    0, -50,
    200  // Width
);

// Update progress
lv_bar_set_value(bar, 75, LV_ANIM_ON);
```

#### Clear Screen

```cpp
Screen.clearScreen();  // Remove all UI elements
```

### Touch Operations

#### Check Touch State

```cpp
bool touched = Touch.isTouched();
uint8_t points = Touch.getTouchPoints();
```

#### Get Raw Touch Coordinates

```cpp
int16_t x, y;
if (Touch.getTouchPoint(x, y)) {
    Serial.printf("Touch at: %d, %d\n", x, y);
}
```

#### Touch Calibration

```cpp
// Set calibration values
Touch.setCalibration(
    0,      // X offset
    0,      // Y offset
    1.0f,   // X scale
    1.0f    // Y scale
);

// Get current calibration
int16_t x_off, y_off;
float x_scale, y_scale;
Touch.getCalibration(x_off, y_off, x_scale, y_scale);
```

#### Noise Filtering

```cpp
Touch.setNoiseThreshold(10);  // Pixels
uint8_t threshold = Touch.getNoiseThreshold();
```

#### Touch Statistics

```cpp
uint32_t count = Touch.getTouchCount();
uint32_t lastTime = Touch.getLastTouchTime();
uint32_t timeSince = Touch.timeSinceLastTouch();
Touch.resetStats();
```

## Advanced Usage

### Double Buffering

```cpp
// Enable double buffering for smoother rendering
Screen.on(true);
```

### Custom User Data with Buttons

```cpp
typedef struct {
    int id;
    const char* name;
} ButtonData;

void button_callback(lv_event_t *e) {
    ButtonData *data = (ButtonData*)lv_event_get_user_data(e);
    Serial.printf("Button %d (%s) clicked\n", data->id, data->name);
}

ButtonData myData = {1, "Button1"};
lv_obj_t *btn = Screen.createButton(
    "Custom",
    LV_ALIGN_CENTER,
    0, 0,
    120, 50,
    button_callback,
    &myData  // User data
);
```

### Memory Diagnostics

```cpp
Screen.printMemoryInfo();
Touch.printInfo();
```

### Access Underlying Objects

```cpp
Arduino_GFX* gfx = Screen.gfx();  // Changed from getGFX()
lv_display_t* disp = Screen.getDisplay();
Arduino_FT3x68* ft3168 = Touch.getFT3168();
```

## Complete Example

```cpp
#include "ESP32-S3-Screen-AMOLED-2.06.h"
#include "ESP32-S3-Touch-AMOLED-2.06.h"

// Create global instances
ScreenClass Screen;
TouchClass Touch;

lv_obj_t *counterLabel;
int counter = 0;

void increment_callback(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        counter++;
        char buf[32];
        snprintf(buf, sizeof(buf), "Count: %d", counter);
        lv_label_set_text(counterLabel, buf);
    }
}

void reset_callback(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        counter = 0;
        lv_label_set_text(counterLabel, "Count: 0");
    }
}

void setup() {
    Serial.begin(115200);
    
    lv_init();
    
    if (!Screen.on()) {
        Serial.println("Screen failed");
        while(1) delay(1000);
    }
    
    if (!Touch.on()) {
        Serial.println("Touch failed");
        while(1) delay(1000);
    }
    
    Touch.setupLVGLInput();
    
    // Create UI
    counterLabel = Screen.createLabel(
        "Count: 0",
        LV_ALIGN_TOP_MID,
        0, 30
    );
    
    Screen.createButton(
        "Increment",
        LV_ALIGN_CENTER,
        0, -30,
        140, 50,
        increment_callback
    );
    
    Screen.createStyledButton(
        "Reset",
        LV_ALIGN_CENTER,
        0, 40,
        140, 50,
        reset_callback,
        lv_color_hex(0xF44336),
        lv_color_hex(0xFFFFFF)
    );
}

void loop() {
    lv_timer_handler();
    delay(5);
}
```

## API Reference

### ScreenClass Methods

**Initialization**
- `bool on(bool useDoubleBuffer = false)` - Initialize display
- `void off()` - Shutdown display

**Display Control**
- `void fillScreen(uint16_t color)` - Fill entire screen
- `void clearScreen()` - Remove all UI elements
- `void sleep()` - Turn off display
- `void wake()` - Turn on display
- `void setBrightness(uint8_t level)` - Set brightness (0-255)

**UI Creation**
- `lv_obj_t* createButton(const char* text, lv_align_t align, int16_t x, int16_t y, int16_t w, int16_t h, lv_event_cb_t cb, void* user_data = nullptr)` - Create button
- `lv_obj_t* createStyledButton(...)` - Create styled button with custom colors
- `lv_obj_t* createLabel(const char* text, lv_align_t align, int16_t x, int16_t y, lv_obj_t* parent = nullptr)` - Create label
- `lv_obj_t* createSlider(lv_align_t align, int16_t x, int16_t y, int16_t w, int32_t min_val, int32_t max_val, int32_t start_val, lv_event_cb_t cb, void* user_data = nullptr)` - Create slider
- `lv_obj_t* createProgressBar(lv_align_t align, int16_t x, int16_t y, int16_t w)` - Create progress bar

**Button Helpers**
- `lv_obj_t* getButtonLabel(lv_obj_t* btn)` - Get button's label object
- `void setButtonText(lv_obj_t* btn, const char* text)` - Update button text

**Information**
- `Arduino_GFX* gfx()` - Get GFX object pointer
- `Arduino_GFX* getGFX()` - Get GFX object pointer (alternative)
- `lv_display_t* getDisplay()` - Get LVGL display object
- `uint32_t width()` - Get screen width
- `uint32_t height()` - Get screen height
- `bool isInitialized()` - Check if initialized
- `uint32_t getFlushCount()` - Get render flush count
- `void printMemoryInfo()` - Print memory statistics

### TouchClass Methods

**Initialization**
- `bool on(uint32_t timeout_ms = 10000)` - Initialize touch controller with timeout
- `void off()` - Shutdown touch controller

**Touch Reading**
- `bool isTouched()` - Check if screen is currently touched
- `uint8_t getTouchPoints()` - Get number of active touch points
- `bool getTouchPoint(int16_t &x, int16_t &y, uint8_t point_index = 0)` - Get touch coordinates
- `lv_indev_t* setupLVGLInput()` - Setup LVGL input integration

**Calibration**
- `void setCalibration(int16_t x_offset, int16_t y_offset, float x_scale = 1.0f, float y_scale = 1.0f)` - Set calibration values
- `void getCalibration(int16_t &x_offset, int16_t &y_offset, float &x_scale, float &y_scale)` - Get calibration values
- `void setNoiseThreshold(uint8_t threshold)` - Set noise filter threshold
- `uint8_t getNoiseThreshold()` - Get noise filter threshold

**Information**
- `Arduino_FT3x68* getFT3168()` - Get touch controller object
- `Arduino_FT3x68* get()` - Get touch controller object (alternative)
- `bool isInitialized()` - Check if initialized
- `uint32_t getTouchCount()` - Get total touch count
- `uint32_t getLastTouchTime()` - Get timestamp of last touch
- `uint32_t timeSinceLastTouch()` - Get milliseconds since last touch
- `uint32_t getInterruptCount()` - Get interrupt count
- `void resetStats()` - Reset statistics counters
- `void printInfo()` - Print touch information

## Important Notes

1. **LVGL Version:** This library requires LVGL v9.x. Use `lv_timer_handler()` instead of `lv_task_handler()`.
2. **Global Instances:** You must declare `ScreenClass Screen;` and `TouchClass Touch;` as global variables in your main sketch.
3. **Initialization Order:** Always initialize in this order: `lv_init()` → `Screen.on()` → `Touch.on()` → `Touch.setupLVGLInput()`.
4. **Memory:** The library uses heap allocation. Monitor memory usage with `printMemoryInfo()`.
5. **Pin Configuration:** Ensure your `pin_config.h` contains all required pin definitions.

## Troubleshooting

### Screen doesn't initialize
- Check power supply
- Verify pin connections in `pin_config.h`
- Ensure all required libraries are installed

### Touch not responding
- Verify I2C pins are correct
- Try increasing initialization timeout: `Touch.on(20000)`
- Check touch calibration values

### Compilation errors
- Ensure you're using LVGL v9.x
- Update all dependency libraries to latest versions
- Check that global instances are declared properly

## License

This library is provided as-is for use with the ESP32-S3 AMOLED 2.06" display.

## Contributing

Contributions are welcome! Please submit issues and pull requests on the project reposito
