#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "Arduino_IIC.h"
#include "Arduino_IIC_Chip.h"
#include "pin_config.h"
#include <lvgl.h>
#include <memory>

class TouchClass {
public:
    TouchClass() {
        instance = this;
    }

    void on() {
        Serial.println("=== TOUCH INIT START ===");
        delay(100);

        // Step 1: power-up and reset the touch IC
        pinMode(TP_RESET, OUTPUT);
        digitalWrite(TP_RESET, LOW);
        delay(10);
        digitalWrite(TP_RESET, HIGH);
        delay(50);

        pinMode(TP_INT, INPUT_PULLUP);
        delay(20);

        // Step 2: start I2C
        Wire.begin(IIC_SDA, IIC_SCL, 400000);
        Serial.printf("I2C begin on SDA=%d SCL=%d\n", IIC_SDA, IIC_SCL);

        // Step 3: scan the I2C bus
        Serial.println("Scanning I2C bus for touch controller...");
        bool found = false;
        uint8_t detectedAddr = 0x00;
        for (uint8_t addr = 1; addr < 127; addr++) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission() == 0) {
                Serial.printf("I2C device found at 0x%02X\n", addr);
                if (addr == FT3168_DEVICE_ADDRESS) {
                    found = true;
                    detectedAddr = addr;
                }
            }
        }

        if (!found) {
            Serial.println("⚠️ No FT3168 found on I2C! Trying anyway...");
            detectedAddr = FT3168_DEVICE_ADDRESS;
        }

        // Step 4: create bus + device objects
        IIC_Bus = std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
        FT3168 = std::make_unique<Arduino_FT3x68>(
            IIC_Bus,
            detectedAddr,
            DRIVEBUS_DEFAULT_VALUE,
            TP_INT,
            touchInterruptStatic
        );

        // Step 5: initialize the chip (multiple tries)
        uint8_t tries = 0;
        while (!FT3168->begin() && tries < 5) {
            Serial.println("FT3168 not responding, retrying...");
            delay(500);
            tries++;
        }

        if (tries >= 5) {
            Serial.println("❌ Touch controller failed to initialize after 5 tries!");
        } else {
            Serial.println("✅ Touch controller initialized successfully!");
        }

        // Step 6: register LVGL input device
        indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, read_cb);
        lv_indev_set_display(indev, lv_display_get_default());

        Serial.printf("LVGL display=%p indev=%p\n", lv_display_get_default(), indev);
        Serial.println("=== TOUCH INIT COMPLETE ===\n");
    }

    lv_indev_t* getIndev() { return indev; }

    bool isTouched() {
        if (!FT3168) return false;
        return FT3168->IIC_Interrupt_Flag || (readRawX() > 0 && readRawY() > 0);
    }

    void getTouchPoint(int32_t &x, int32_t &y) {
        if (!FT3168) { x = y = 0; return; }
        x = readRawX();
        y = readRawY();
    }

private:
    std::shared_ptr<Arduino_HWIIC> IIC_Bus;
    std::unique_ptr<Arduino_FT3x68> FT3168;
    lv_indev_t* indev = nullptr;
    static TouchClass* instance;
    static bool usePolling; // fallback mode flag

    static void touchInterruptStatic() {
        if (instance && instance->FT3168) {
            instance->FT3168->IIC_Interrupt_Flag = true;
            // Uncomment for noisy debug:
            // Serial.println("[INTERRUPT] Touch detected!");
        }
    }

    static void read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
        if (!instance || !instance->FT3168) {
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        // Interrupt-based mode
        if (instance->FT3168->IIC_Interrupt_Flag) {
            instance->FT3168->IIC_Interrupt_Flag = false;
            int32_t x = instance->readRawX();
            int32_t y = instance->readRawY();
            if (x > 0 && y > 0) {
                data->state = LV_INDEV_STATE_PRESSED;
                data->point.x = x;
                data->point.y = y;
                Serial.printf("[TOUCH INT] X=%d Y=%d\n", x, y);
                return;
            }
        }

        // Polling fallback (runs every tick)
        int32_t x = instance->readRawX();
        int32_t y = instance->readRawY();
        if (x > 0 && y > 0) {
            data->state = LV_INDEV_STATE_PRESSED;
            data->point.x = x;
            data->point.y = y;
            if (!usePolling) {
                Serial.println("⚠️ Using polling fallback mode (no INT detected)");
                usePolling = true;
            }
            Serial.printf("[TOUCH POLL] X=%d Y=%d\n", x, y);
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    int32_t readRawX() {
        if (!FT3168) return 0;
        int32_t val = FT3168->IIC_Read_Device_Value(
            FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        return val;
    }

    int32_t readRawY() {
        if (!FT3168) return 0;
        int32_t val = FT3168->IIC_Read_Device_Value(
            FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
        return val;
    }
};

// Static member definitions
TouchClass* TouchClass::instance = nullptr;
bool TouchClass::usePolling = false;

// Global object
extern TouchClass Touch;
