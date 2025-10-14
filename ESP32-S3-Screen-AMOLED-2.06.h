#pragma once
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "lvgl.h"
#include "pin_config.h"

class ScreenClass {
private:
    Arduino_DataBus *bus = nullptr;
    Arduino_GFX *gfx = nullptr;
    lv_display_t *disp = nullptr;
    lv_color_t *disp_draw_buf = nullptr;
    lv_color_t *disp_draw_buf2 = nullptr;
    
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    uint32_t bufSize = 0;
    uint8_t brightness = 255;
    bool initialized = false;
    bool doubleBuffer = false;
    
    // Statistics
    uint32_t flushCount = 0;
    uint32_t lastFlushTime = 0;

public:
    // Initialization with optional double buffering
    bool on(bool useDoubleBuffer = false) {
        if (initialized) {
            Serial.println("Screen already initialized");
            return true;
        }

        doubleBuffer = useDoubleBuffer;
        
        // Initialize bus
        bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
                                    LCD_SDIO2, LCD_SDIO3);
        if (!bus) {
            Serial.println("Failed to create bus");
            return false;
        }

        // Initialize display
        gfx = new Arduino_CO5300(bus, LCD_RESET, 0, LCD_WIDTH, LCD_HEIGHT, 22, 0, 0, 0);
        if (!gfx) {
            Serial.println("Failed to create GFX");
            delete bus;
            bus = nullptr;
            return false;
        }

        if (!gfx->begin()) {
            Serial.println("Failed to begin GFX");
            delete gfx;
            delete bus;
            gfx = nullptr;
            bus = nullptr;
            return false;
        }

        gfx->fillScreen(RGB565_BLACK);
        
        screenWidth = gfx->width();
        screenHeight = gfx->height();
        bufSize = screenWidth * 50;

        // Allocate primary buffer
        disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), 
                                                       MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (!disp_draw_buf) {
            disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), 
                                                           MALLOC_CAP_8BIT);
        }
        
        if (!disp_draw_buf) {
            Serial.println("Failed to allocate display buffer");
            delete gfx;
            delete bus;
            gfx = nullptr;
            bus = nullptr;
            return false;
        }

        // Allocate secondary buffer if double buffering enabled
        if (doubleBuffer) {
            disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), 
                                                            MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (!disp_draw_buf2) {
                disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), 
                                                                MALLOC_CAP_8BIT);
            }
            if (!disp_draw_buf2) {
                Serial.println("Failed to allocate second buffer, using single buffer");
                doubleBuffer = false;
            }
        }

        // Create LVGL display
        disp = lv_display_create(screenWidth, screenHeight);
        if (!disp) {
            Serial.println("Failed to create LVGL display");
            free(disp_draw_buf);
            if (disp_draw_buf2) free(disp_draw_buf2);
            delete gfx;
            delete bus;
            disp_draw_buf = nullptr;
            disp_draw_buf2 = nullptr;
            gfx = nullptr;
            bus = nullptr;
            return false;
        }

        // Set flush callback
        lv_display_set_flush_cb(disp, [](lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
            uint32_t w = lv_area_get_width(area);
            uint32_t h = lv_area_get_height(area);
            
            if (Screen.gfx()) {
                Screen.lastFlushTime = millis();
                static_cast<Arduino_CO5300*>(Screen.gfx())->draw16bitRGBBitmap(
                    area->x1, area->y1, (uint16_t *)px_map, w, h);
                Screen.flushCount++;
            }
            
            lv_disp_flush_ready(disp);
        });

        // Set buffers
        size_t bufferSize = bufSize * sizeof(lv_color_t);
        lv_display_set_buffers(disp, disp_draw_buf, disp_draw_buf2, bufferSize, 
                              LV_DISPLAY_RENDER_MODE_PARTIAL);

        initialized = true;
        Serial.printf("Screen initialized: %dx%d, Buffer: %d bytes%s\n", 
                     screenWidth, screenHeight, bufferSize, 
                     doubleBuffer ? " (double buffered)" : "");
        
        return true;
    }

    // Shutdown display
    void off() {
        if (!initialized) return;

        if (disp) {
            lv_display_delete(disp);
            disp = nullptr;
        }
        
        if (disp_draw_buf) {
            free(disp_draw_buf);
            disp_draw_buf = nullptr;
        }
        
        if (disp_draw_buf2) {
            free(disp_draw_buf2);
            disp_draw_buf2 = nullptr;
        }
        
        if (gfx) {
            delete gfx;
            gfx = nullptr;
        }
        
        if (bus) {
            delete bus;
            bus = nullptr;
        }

        initialized = false;
        Serial.println("Screen shutdown complete");
    }

    // Getters
    Arduino_GFX* gfx() { return gfx; }
    Arduino_GFX* getGFX() { return gfx; }
    lv_display_t* getDisplay() { return disp; }
    uint32_t width() const { return screenWidth; }
    uint32_t height() const { return screenHeight; }
    bool isInitialized() const { return initialized; }
    uint32_t getFlushCount() const { return flushCount; }
    uint32_t getLastFlushTime() const { return lastFlushTime; }

    // Brightness control
    void setBrightness(uint8_t level) {
        brightness = level;
        // Implement hardware brightness control if available
        // gfx->setBrightness(brightness);
    }
    
    uint8_t getBrightness() const { return brightness; }

    // Screen operations
    void fillScreen(uint16_t color) {
        if (gfx) gfx->fillScreen(color);
    }

    void sleep() {
        if (gfx) gfx->displayOff();
    }

    void wake() {
        if (gfx) gfx->displayOn();
    }

    // ===== ENHANCED BUTTON CREATION =====
    lv_obj_t* createButton(const char* text, lv_align_t align, int16_t x, int16_t y,
                           int16_t w, int16_t h, lv_event_cb_t cb, void* user_data = nullptr) {
        if (!initialized || !disp) return nullptr;

        lv_obj_t *btn = lv_button_create(lv_scr_act());
        if (!btn) return nullptr;

        lv_obj_set_size(btn, w, h);
        lv_obj_align(btn, align, x, y);
        
        if (cb) {
            lv_obj_add_event_cb(btn, cb, LV_EVENT_ALL, user_data);
        }

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);

        return btn;
    }

    // Styled button with custom colors
    lv_obj_t* createStyledButton(const char* text, lv_align_t align, int16_t x, int16_t y,
                                 int16_t w, int16_t h, lv_event_cb_t cb,
                                 lv_color_t bg_color, lv_color_t text_color, 
                                 void* user_data = nullptr) {
        lv_obj_t *btn = createButton(text, align, x, y, w, h, cb, user_data);
        if (!btn) return nullptr;

        lv_obj_set_style_bg_color(btn, bg_color, LV_PART_MAIN);
        
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        if (label) {
            lv_obj_set_style_text_color(label, text_color, LV_PART_MAIN);
        }

        return btn;
    }

    // Button with icon
    lv_obj_t* createIconButton(const char* icon_text, lv_align_t align, int16_t x, int16_t y,
                               int16_t w, int16_t h, lv_event_cb_t cb, void* user_data = nullptr) {
        lv_obj_t *btn = createButton(icon_text, align, x, y, w, h, cb, user_data);
        if (!btn) return nullptr;

        lv_obj_t *label = lv_obj_get_child(btn, 0);
        if (label) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
        }

        return btn;
    }

    // Get button label
    lv_obj_t* getButtonLabel(lv_obj_t* btn) {
        return btn ? lv_obj_get_child(btn, 0) : nullptr;
    }

    // Set button text
    void setButtonText(lv_obj_t* btn, const char* text) {
        lv_obj_t *label = getButtonLabel(btn);
        if (label) lv_label_set_text(label, text);
    }

    // ===== LABEL CREATION =====
    lv_obj_t* createLabel(const char* text, lv_align_t align, int16_t x, int16_t y,
                         lv_obj_t* parent = nullptr) {
        if (!initialized) return nullptr;

        lv_obj_t *label = lv_label_create(parent ? parent : lv_scr_act());
        if (!label) return nullptr;

        lv_label_set_text(label, text);
        lv_obj_align(label, align, x, y);

        return label;
    }

    // ===== SLIDER CREATION =====
    lv_obj_t* createSlider(lv_align_t align, int16_t x, int16_t y, int16_t w,
                          int32_t min_val, int32_t max_val, int32_t start_val,
                          lv_event_cb_t cb, void* user_data = nullptr) {
        if (!initialized) return nullptr;

        lv_obj_t *slider = lv_slider_create(lv_scr_act());
        if (!slider) return nullptr;

        lv_obj_set_size(slider, w, 20);
        lv_obj_align(slider, align, x, y);
        lv_slider_set_range(slider, min_val, max_val);
        lv_slider_set_value(slider, start_val, LV_ANIM_OFF);

        if (cb) {
            lv_obj_add_event_cb(slider, cb, LV_EVENT_VALUE_CHANGED, user_data);
        }

        return slider;
    }

    // ===== PROGRESS BAR =====
    lv_obj_t* createProgressBar(lv_align_t align, int16_t x, int16_t y, int16_t w) {
        if (!initialized) return nullptr;

        lv_obj_t *bar = lv_bar_create(lv_scr_act());
        if (!bar) return nullptr;

        lv_obj_set_size(bar, w, 20);
        lv_obj_align(bar, align, x, y);
        lv_bar_set_value(bar, 0, LV_ANIM_OFF);

        return bar;
    }

    // ===== UTILITY FUNCTIONS =====
    void clearScreen() {
        if (!initialized) return;
        lv_obj_clean(lv_scr_act());
    }

    void printMemoryInfo() {
        Serial.printf("=== Screen Memory Info ===\n");
        Serial.printf("Screen: %dx%d\n", screenWidth, screenHeight);
        Serial.printf("Buffer size: %d bytes\n", bufSize * sizeof(lv_color_t));
        Serial.printf("Double buffered: %s\n", doubleBuffer ? "Yes" : "No");
        Serial.printf("Total allocated: %d bytes\n", 
                     bufSize * sizeof(lv_color_t) * (doubleBuffer ? 2 : 1));
        Serial.printf("Flush count: %d\n", flushCount);
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    }

    // Destructor
    ~ScreenClass() {
        off();
    }
};

extern ScreenClass Screen;