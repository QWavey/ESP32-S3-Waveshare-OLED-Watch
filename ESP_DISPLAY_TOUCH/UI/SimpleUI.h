#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <functional>

// Global helper functions
// Note: Call SimpleUI_init() AFTER your display/touch initialization
void SimpleUI_init();
void SimpleUI_update();

// Forward declarations
class TextBox;

// ==================== Screen Class ====================
class UIScreen {
public:
    UIScreen();
    ~UIScreen();
    
    void load();
    void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
    lv_obj_t* getObject() { return screen; }
    
private:
    lv_obj_t* screen = nullptr;
};

// ==================== Label Class ====================
class UILabel {
public:
    UILabel();
    ~UILabel();
    
    void create(lv_obj_t* parent, int x, int y, int w = LV_SIZE_CONTENT);
    void setText(const char* text);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setFontSize(uint8_t size);
    void hide();
    void show();
    lv_obj_t* getObject() { return label; }
    
private:
    lv_obj_t* label;
};

// ==================== Button Class ====================
class UIButton {
public:
    UIButton();
    ~UIButton();
    
    void create(lv_obj_t* parent, const char* text, int x, int y, int w, int h);
    void onClick(std::function<void()> callback);
    void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
    void setText(const char* text);
    void hide();
    void show();
    void enable();
    void disable();
    lv_obj_t* getObject() { return btn; }
    
private:
    lv_obj_t* btn;
    lv_obj_t* label;
    std::function<void()> clickCallback;
    
    static void event_handler(lv_event_t* e);
};

// ==================== TextBox Class ====================
class UITextBox {
public:
    UITextBox();
    ~UITextBox();
    
    void create(lv_obj_t* parent, int x, int y, int w, int h = 40);
    void setPlaceholder(const char* text);
    void setPasswordMode(bool enabled);
    void setText(const char* text);
    String getText();
    void clear();
    void focus();
    void onClicked(std::function<void()> callback);
    void onSubmit(std::function<void(String)> callback);
    lv_obj_t* getObject() { return textarea; }
    
    friend class UIKeyboard;
    
private:
    lv_obj_t* textarea;
    std::function<void()> clickedCallback;
    std::function<void(String)> submitCallback;
    
    static void event_handler(lv_event_t* e);
    void triggerSubmit();
};

// ==================== Keyboard Class ====================
class UIKeyboard {
public:
    UIKeyboard();
    ~UIKeyboard();
    
    // Spawn keyboard at bottom of screen (x, y ignored - always bottom center)
    // Keyboard will take full width (edge to edge) and half screen height
    // Buttons will be bigger to fill the space
    void spawn(lv_obj_t* parent, int x, int y, int w, int h);
    void linkTo(UITextBox* textbox);
    void hide();
    bool isVisible();
    void onEnter(std::function<void(String)> callback);
    void onCancel(std::function<void()> callback);
    lv_obj_t* getObject() { return keyboard; }
    
private:
    lv_obj_t* keyboard;
    UITextBox* linkedTextBox;
    std::function<void(String)> enterCallback;
    std::function<void()> cancelCallback;
    bool visible;
    
    static void event_handler(lv_event_t* e);
};
