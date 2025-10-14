# ESP32-S3 AMOLED 2.06" Display Library

A comprehensive library for controlling the ESP32-S3 AMOLED 2.06" display with FT3168 touch controller and LVGL integration.

## Installation

1. Download or clone this repository
2. Copy the files to your Arduino libraries directory:
   - Windows: `Documents\Arduino\libraries\`
   - Mac: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`
3. Restart Arduino IDE
4. Install dependencies:
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

## LINKS:

ARDUINO: https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.06/tree/main/examples/Arduino-v3.2.0/libraries

ESP-IDF: https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-2.06/tree/main/examples/ESP-IDF-v5.4.2


## Basic Usage

### Screen Initialization

```cpp
#include "ESP32-S3-Screen-AMOLED-2.06.h"
#include "ESP32-S3-Touch-AMOLED-2.06.h"

void setup() {
    Serial.begin(115200);
    
    // Initialize LVGL
    lv_init();
    
    // Initialize screen (optional double buffering)
    if (!Screen.on(false)) {
        Serial.println("Screen initialization failed");
        while(1);
    }
    
    // Initialize touch
    if (!Touch.on()) {
        Serial.println("Touch initialization failed");
        while(1);
    }
    
    // Setup LVGL touch input
    Touch.setupLVGLInput();
}

void loop() {
    lv_task_handler();
    delay(5);
}
```

### Screen Operations

#### Get Display Information
```cpp
uint32_t width = Screen.width();
uint32_t height = Screen.height();
bool ready = Screen.isInitialized();
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
    button_callback          // Callback function
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
    lv_color_hex(0xFFFFFF)   // Text color
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
    0, 20
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
    slider_callback
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

#### Touch Power Management
```cpp
Touch.sleep();  // Enter sleep mode
Touch.wake();   // Exit sleep mode
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
Arduino_GFX* gfx = Screen.getGFX();
lv_display_t* disp = Screen.getDisplay();
Arduino_IIC* ft3168 = Touch.getFT3168();
```

## Complete Example

```cpp
#include "ESP32-S3-Screen-AMOLED-2.06.h"
#include "ESP32-S3-Touch-AMOLED-2.06.h"

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
        while(1);
    }
    
    if (!Touch.on()) {
        Serial.println("Touch failed");
        while(1);
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
    lv_task_handler();
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
- `lv_obj_t* createButton(...)` - Create button
- `lv_obj_t* createStyledButton(...)` - Create styled button
- `lv_obj_t* createIconButton(...)` - Create icon button
- `lv_obj_t* createLabel(...)` - Create label
- `lv_obj_t* createSlider(...)` - Create slider
- `lv_obj_t* createProgressBar(...)` - Create progress bar

**Information**
- `uint32_t width()` - Get screen width
- `uint32_t height()` - Get screen height
- `bool isInitialized()` - Check if initialized
- `void printMemoryInfo()` - Print memory statistics

### TouchClass Methods

**Initialization**
- `bool on(uint32_t timeout_ms = 10000)` - Initialize touch
- `void off()` - Shutdown touch

**Touch Reading**
- `bool isTouched()` - Check if screen is touched
- `uint8_t getTouchPoints()` - Get number of touch points
- `bool getTouchPoint(int16_t &x, int16_t &y)` - Get touch coordinates
- `lv_indev_t* setupLVGLInput()` - Setup LVGL integration

**Calibration**
- `void setCalibration(...)` - Set calibration values
- `void getCalibration(...)` - Get calibration values
- `void setNoiseThreshold(uint8_t)` - Set noise filter threshold

**Power Management**
- `bool sleep()` - Enter sleep mode
- `bool wake()` - Exit sleep mode

**Statistics**
- `uint32_t getTouchCount()` - Get total touch count
- `uint32_t timeSinceLastTouch()` - Time since last touch
- `void resetStats()` - Reset statistics
- `void printInfo()` - Print touch information

## Troubleshooting

**
