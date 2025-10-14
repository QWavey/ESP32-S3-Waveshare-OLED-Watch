#pragma once
#include <Wire.h>
#include <Arduino.h>
#include "Arduino_DriveBus_Library.h"
#include "XPowersLib.h"
#include "pin_config.h"
#include "lvgl.h"

class TouchClass {
private:
    std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus = nullptr;
    std::unique_ptr<Arduino_IIC> FT3168 = nullptr;
    
    bool initialized = false;
    uint32_t touchCount = 0;
    uint32_t lastTouchTime = 0;
    
    // Touch calibration
    int16_t calibration_x_offset = 0;
    int16_t calibration_y_offset = 0;
    float calibration_x_scale = 1.0f;
    float calibration_y_scale = 1.0f;
    
    // Touch filtering
    int16_t last_x = -1;
    int16_t last_y = -1;
    uint8_t noise_threshold = 10;
    
    // Statistics
    uint32_t interrupt_count = 0;

public:
    // Initialize touch controller
    bool on(uint32_t timeout_ms = 10000) {
        if (initialized) {
            Serial.println("Touch already initialized");
            return true;
        }

        // Initialize I2C
        if (!Wire.begin(IIC_SDA, IIC_SCL)) {
            Serial.println("Failed to initialize I2C");
            return false;
        }
        
        Wire.setClock(400000); // 400kHz I2C speed

        // Create I2C bus
        IIC_Bus = std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
        if (!IIC_Bus) {
            Serial.println("Failed to create IIC bus");
            return false;
        }

        // Create touch controller
        FT3168 = std::make_unique<Arduino_FT3x68>(
            IIC_Bus, 
            FT3168_DEVICE_ADDRESS,
            DRIVEBUS_DEFAULT_VALUE, 
            TP_INT,
            std::bind(&TouchClass::interrupt_cb, this)
        );

        if (!FT3168) {
            Serial.println("Failed to create FT3168 controller");
            IIC_Bus.reset();
            return false;
        }

        // Try to initialize with timeout
        uint32_t start = millis();
        while (!FT3168->begin()) {
            if (millis() - start > timeout_ms) {
                Serial.println("Touch initialization timeout");
                FT3168.reset();
                IIC_Bus.reset();
                return false;
            }
            delay(100);
        }

        // Set power mode
        if (!FT3168->IIC_Write_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
            FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR)) {
            Serial.println("Warning: Failed to set power mode");
        }

        initialized = true;
        Serial.println("Touch initialized successfully");
        
        return true;
    }

    // Shutdown touch controller
    void off() {
        if (!initialized) return;

        // Set to sleep mode before shutdown
        if (FT3168) {
            FT3168->IIC_Write_Device_State(
                FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_SLEEP
            );
        }

        FT3168.reset();
        IIC_Bus.reset();
        initialized = false;
        
        Serial.println("Touch shutdown complete");
    }

    // Interrupt callback
    void interrupt_cb() {
        if (FT3168) {
            FT3168->IIC_Interrupt_Flag = true;
            interrupt_count++;
        }
    }

    // Read touch input for LVGL
    void lvgl_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
        if (!initialized || !FT3168) {
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        uint8_t touched = FT3168->IIC_Read_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POINTS_NUM
        );

        if (touched > 0) {
            int16_t x = FT3168->IIC_Read_Device_State(
                FT3168->Arduino_IIC_Touch::Device::TOUCH_COORDINATE_X
            );
            int16_t y = FT3168->IIC_Read_Device_State(
                FT3168->Arduino_IIC_Touch::Device::TOUCH_COORDINATE_Y
            );

            // Apply calibration
            x = (int16_t)((x + calibration_x_offset) * calibration_x_scale);
            y = (int16_t)((y + calibration_y_offset) * calibration_y_scale);

            // Apply noise filter
            if (last_x != -1 && last_y != -1) {
                int16_t dx = abs(x - last_x);
                int16_t dy = abs(y - last_y);
                
                if (dx < noise_threshold && dy < noise_threshold) {
                    // Too small movement, keep last position
                    x = last_x;
                    y = last_y;
                }
            }

            last_x = x;
            last_y = y;
            lastTouchTime = millis();
            touchCount++;

            data->point.x = x;
            data->point.y = y;
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            last_x = -1;
            last_y = -1;
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    // Setup LVGL input device
    lv_indev_t* setupLVGLInput() {
        if (!initialized) {
            Serial.println("Touch not initialized");
            return nullptr;
        }

        lv_indev_t *touch_indev = lv_indev_create();
        if (!touch_indev) {
            Serial.println("Failed to create LVGL input device");
            return nullptr;
        }

        lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(touch_indev, [](lv_indev_t *indev, lv_indev_data_t *data) {
            Touch.lvgl_touch_read(indev, data);
        });

        Serial.println("LVGL touch input configured");
        return touch_indev;
    }

    // Get raw touch data
    bool getTouchPoint(int16_t &x, int16_t &y, uint8_t point_index = 0) {
        if (!initialized || !FT3168) return false;

        uint8_t touched = FT3168->IIC_Read_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POINTS_NUM
        );

        if (touched > point_index) {
            x = FT3168->IIC_Read_Device_State(
                FT3168->Arduino_IIC_Touch::Device::TOUCH_COORDINATE_X
            );
            y = FT3168->IIC_Read_Device_State(
                FT3168->Arduino_IIC_Touch::Device::TOUCH_COORDINATE_Y
            );
            return true;
        }
        return false;
    }

    // Multi-touch support
    uint8_t getTouchPoints() {
        if (!initialized || !FT3168) return 0;
        
        return FT3168->IIC_Read_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POINTS_NUM
        );
    }

    // Calibration functions
    void setCalibration(int16_t x_offset, int16_t y_offset, 
                       float x_scale = 1.0f, float y_scale = 1.0f) {
        calibration_x_offset = x_offset;
        calibration_y_offset = y_offset;
        calibration_x_scale = x_scale;
        calibration_y_scale = y_scale;
        Serial.printf("Touch calibration set: offset(%d,%d) scale(%.2f,%.2f)\n",
                     x_offset, y_offset, x_scale, y_scale);
    }

    void getCalibration(int16_t &x_offset, int16_t &y_offset, 
                       float &x_scale, float &y_scale) {
        x_offset = calibration_x_offset;
        y_offset = calibration_y_offset;
        x_scale = calibration_x_scale;
        y_scale = calibration_y_scale;
    }

    // Noise filter threshold
    void setNoiseThreshold(uint8_t threshold) {
        noise_threshold = threshold;
    }

    uint8_t getNoiseThreshold() const {
        return noise_threshold;
    }

    // Power management
    bool sleep() {
        if (!initialized || !FT3168) return false;
        
        return FT3168->IIC_Write_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
            FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_SLEEP
        );
    }

    bool wake() {
        if (!initialized || !FT3168) return false;
        
        return FT3168->IIC_Write_Device_State(
            FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
            FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR
        );
    }

    // Getters
    Arduino_IIC* getFT3168() { return FT3168.get(); }
    Arduino_IIC* get() { return FT3168.get(); }
    bool isInitialized() const { return initialized; }
    uint32_t getTouchCount() const { return touchCount; }
    uint32_t getLastTouchTime() const { return lastTouchTime; }
    uint32_t getInterruptCount() const { return interrupt_count; }

    // Check if currently touched
    bool isTouched() {
        return getTouchPoints() > 0;
    }

    // Get time since last touch
    uint32_t timeSinceLastTouch() {
        if (lastTouchTime == 0) return 0xFFFFFFFF;
        return millis() - lastTouchTime;
    }

    // Reset statistics
    void resetStats() {
        touchCount = 0;
        interrupt_count = 0;
        lastTouchTime = 0;
    }

    // Print debug information
    void printInfo() {
        Serial.printf("=== Touch Info ===\n");
        Serial.printf("Initialized: %s\n", initialized ? "Yes" : "No");
        Serial.printf("Touch count: %d\n", touchCount);
        Serial.printf("Interrupt count: %d\n", interrupt_count);
        Serial.printf("Last touch: %d ms ago\n", timeSinceLastTouch());
        Serial.printf("Calibration: offset(%d,%d) scale(%.2f,%.2f)\n",
                     calibration_x_offset, calibration_y_offset,
                     calibration_x_scale, calibration_y_scale);
        Serial.printf("Noise threshold: %d\n", noise_threshold);
        Serial.printf("Currently touched: %s\n", isTouched() ? "Yes" : "No");
    }

    // Destructor
    ~TouchClass() {
        off();
    }
};

extern TouchClass Touch;
