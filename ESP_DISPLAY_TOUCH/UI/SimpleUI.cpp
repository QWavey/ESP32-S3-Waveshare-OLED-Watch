#ifndef SIMPLEUI_H
#define SIMPLEUI_H

#include <Arduino.h>
#include <lvgl.h>
#include <functional>

// ==================== Global Functions ====================
void SimpleUI_init();
void SimpleUI_update();

// ==================== Screen Class ====================
class UIScreen {
private:
    lv_obj_t* screen;

public:
    UIScreen();
    ~UIScreen();
    void load();
    void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
    lv_obj_t* getObject() { return screen; }
};

// ==================== Label Class ====================
class UILabel {
private:
    lv_obj_t* label;

public:
    UILabel();
    ~UILabel();
    void create(lv_obj_t* parent = nullptr, int x = 0, int y = 0, int w = LV_SIZE_CONTENT);
    void setText(const char* text);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setFontSize(uint8_t size);
    void hide();
    void show();
    lv_obj_t* getObject() { return label; }
};

// ==================== Button Class ====================
class UIButton {
private:
    lv_obj_t* btn;
    lv_obj_t* label;
    std::function<void()> clickCallback;

    static void event_handler(lv_event_t* e);

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
};

// ==================== TextBox Class ====================
class UITextBox {
private:
    lv_obj_t* textarea;
    std::function<void()> clickedCallback;
    std::function<void(String)> submitCallback;

    static void event_handler(lv_event_t* e);
    
public:
    UITextBox();
    ~UITextBox();
    void create(lv_obj_t* parent, int x, int y, int w, int h);
    void setPlaceholder(const char* text);
    void setPasswordMode(bool enabled);
    void setText(const char* text);
    String getText();
    void clear();
    void focus();
    void onClicked(std::function<void()> callback);
    void onSubmit(std::function<void(String)> callback);
    void setCursorColor(uint8_t r, uint8_t g, uint8_t b);
    void submitText();  // Public method to trigger submit
    lv_obj_t* getObject() { return textarea; }
};

// ==================== Keyboard Class ====================
class UIKeyboard {
private:
    lv_obj_t* keyboard;
    UITextBox* linkedTextBox;
    bool visible;
    std::function<void(String)> enterCallback;
    std::function<void()> cancelCallback;

    static void event_handler(lv_event_t* e);

public:
    UIKeyboard();
    ~UIKeyboard();
    void spawn(lv_obj_t* parent, int x, int y, int w, int h);
    void linkTo(UITextBox* textbox);
    void hide();
    bool isVisible();
    void onEnter(std::function<void(String)> callback);
    void onCancel(std::function<void()> callback);
    void setButtonSize(int min_width, int min_height);
    lv_obj_t* getObject() { return keyboard; }
};

// ==================== Implementation ====================

void SimpleUI_init() {}

void SimpleUI_update() {
    lv_task_handler();
}

UIScreen::UIScreen() : screen(nullptr) {}

UIScreen::~UIScreen() {
    if (screen && lv_obj_is_valid(screen)) {
        lv_obj_del(screen);
    }
}

void UIScreen::load() {
    if (!screen) {
        screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(screen, lv_color_hex(0x1a1a1a), 0);
        lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    }
    lv_scr_load(screen);
}

void UIScreen::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
    if (!screen) {
        screen = lv_obj_create(NULL);
    }
    uint32_t color = (r << 16) | (g << 8) | b;
    lv_obj_set_style_bg_color(screen, lv_color_hex(color), 0);
}

UILabel::UILabel() : label(nullptr) {}

UILabel::~UILabel() {
    if (label && lv_obj_is_valid(label)) {
        lv_obj_del(label);
    }
}

void UILabel::create(lv_obj_t* parent, int x, int y, int w) {
    if (!parent) parent = lv_scr_act();
    
    label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    
    if (w != LV_SIZE_CONTENT) {
        lv_obj_set_width(label, w);
    }
    
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    lv_label_set_text(label, "Label");
}

void UILabel::setText(const char* text) {
    if (label) {
        lv_label_set_text(label, text);
    }
}

void UILabel::setColor(uint8_t r, uint8_t g, uint8_t b) {
    if (label) {
        uint32_t color = (r << 16) | (g << 8) | b;
        lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    }
}

void UILabel::setFontSize(uint8_t size) {
    if (label) {
        if (size <= 12) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
        } else if (size <= 14) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        } else if (size <= 16) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
        } else if (size <= 20) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
        } else {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        }
    }
}

void UILabel::hide() {
    if (label) lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
}

void UILabel::show() {
    if (label) lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
}

UIButton::UIButton() : btn(nullptr), label(nullptr) {}

UIButton::~UIButton() {
    if (btn && lv_obj_is_valid(btn)) {
        lv_obj_del(btn);
    }
}

void UIButton::create(lv_obj_t* parent, const char* text, int x, int y, int w, int h) {
    if (!parent) parent = lv_scr_act();
    
    btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_pad_all(btn, 10, 0);
    
    label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
}

void UIButton::onClick(std::function<void()> callback) {
    clickCallback = callback;
    if (btn) {
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, this);
    }
}

void UIButton::event_handler(lv_event_t* e) {
    UIButton* instance = (UIButton*)lv_event_get_user_data(e);
    if (instance && instance->clickCallback) {
        instance->clickCallback();
    }
}

void UIButton::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
    if (btn) {
        uint32_t color = (r << 16) | (g << 8) | b;
        lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
    }
}

void UIButton::setText(const char* text) {
    if (label) {
        lv_label_set_text(label, text);
    }
}

void UIButton::hide() {
    if (btn) lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
}

void UIButton::show() {
    if (btn) lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);
}

void UIButton::enable() {
    if (btn) lv_obj_clear_state(btn, LV_STATE_DISABLED);
}

void UIButton::disable() {
    if (btn) lv_obj_add_state(btn, LV_STATE_DISABLED);
}

UITextBox::UITextBox() : textarea(nullptr) {}

UITextBox::~UITextBox() {
    if (textarea && lv_obj_is_valid(textarea)) {
        lv_obj_del(textarea);
    }
}

void UITextBox::create(lv_obj_t* parent, int x, int y, int w, int h) {
    if (!parent) parent = lv_scr_act();
    
    textarea = lv_textarea_create(parent);
    lv_obj_set_pos(textarea, x, y);
    lv_obj_set_size(textarea, w, h);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_max_length(textarea, 64);
    
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x555555), 0);
    lv_obj_set_style_border_width(textarea, 2, 0);
    lv_obj_set_style_text_color(textarea, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_pad_all(textarea, 8, 0);
    
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x2196F3), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(textarea, 3, LV_STATE_FOCUSED);
    
    lv_obj_set_style_border_color(textarea, lv_color_hex(0xffffff), LV_PART_CURSOR);
    lv_obj_set_style_border_width(textarea, 2, LV_PART_CURSOR);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0xffffff), LV_PART_CURSOR);
    
    lv_obj_set_style_anim_time(textarea, 500, LV_PART_CURSOR);
    
    lv_obj_add_event_cb(textarea, event_handler, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(textarea, event_handler, LV_EVENT_READY, this);
}

void UITextBox::event_handler(lv_event_t* e) {
    UITextBox* instance = (UITextBox*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_CLICKED) {
        Serial.println("TextBox: Clicked");
        if (instance && instance->clickedCallback) {
            instance->clickedCallback();
        }
    }
    else if (code == LV_EVENT_READY) {
        Serial.println("TextBox: Ready (Enter pressed)");
        if (instance) {
            instance->submitText();
        }
    }
}

void UITextBox::submitText() {
    if (submitCallback) {
        submitCallback(getText());
    }
}

void UITextBox::setPlaceholder(const char* text) {
    if (textarea) {
        lv_textarea_set_placeholder_text(textarea, text);
    }
}

void UITextBox::setPasswordMode(bool enabled) {
    if (textarea) {
        lv_textarea_set_password_mode(textarea, enabled);
    }
}

void UITextBox::setText(const char* text) {
    if (textarea) {
        lv_textarea_set_text(textarea, text);
    }
}

String UITextBox::getText() {
    if (textarea) {
        return String(lv_textarea_get_text(textarea));
    }
    return "";
}

void UITextBox::clear() {
    if (textarea) {
        lv_textarea_set_text(textarea, "");
    }
}

void UITextBox::focus() {
    if (textarea) {
        lv_obj_add_state(textarea, LV_STATE_FOCUSED);
    }
}

void UITextBox::onClicked(std::function<void()> callback) {
    clickedCallback = callback;
}

void UITextBox::onSubmit(std::function<void(String)> callback) {
    submitCallback = callback;
}

void UITextBox::setCursorColor(uint8_t r, uint8_t g, uint8_t b) {
    if (textarea) {
        uint32_t color = (r << 16) | (g << 8) | b;
        lv_obj_set_style_border_color(textarea, lv_color_hex(color), LV_PART_CURSOR);
        lv_obj_set_style_bg_color(textarea, lv_color_hex(color), LV_PART_CURSOR);
    }
}

UIKeyboard::UIKeyboard() : keyboard(nullptr), linkedTextBox(nullptr), visible(false) {}

UIKeyboard::~UIKeyboard() {
    if (keyboard && lv_obj_is_valid(keyboard)) {
        lv_obj_del(keyboard);
    }
}

void UIKeyboard::spawn(lv_obj_t* parent, int x, int y, int w, int h) {
    if (!parent) parent = lv_scr_act();
    
    if (keyboard && lv_obj_is_valid(keyboard)) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }
    
    keyboard = lv_keyboard_create(parent);
    
    lv_obj_set_size(keyboard, w, h);
    lv_obj_set_pos(keyboard, x, y);
    lv_obj_move_foreground(keyboard);
    
    lv_obj_set_style_pad_all(keyboard, 0, 0);
    lv_obj_set_style_pad_gap(keyboard, 2, 0);
    
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(keyboard, 0, 0);
    
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x333333), LV_PART_ITEMS);
    lv_obj_set_style_radius(keyboard, 6, LV_PART_ITEMS);
    lv_obj_set_style_text_color(keyboard, lv_color_hex(0xffffff), LV_PART_ITEMS);
    
    lv_obj_set_style_pad_all(keyboard, 8, LV_PART_ITEMS);
    lv_obj_set_style_pad_gap(keyboard, 2, LV_PART_ITEMS);
    
    lv_obj_set_style_text_font(keyboard, &lv_font_montserrat_20, LV_PART_ITEMS);
    
    lv_obj_set_style_min_height(keyboard, 40, LV_PART_ITEMS);
    lv_obj_set_style_min_width(keyboard, 40, LV_PART_ITEMS);
    
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x2196F3), LV_PART_ITEMS | LV_STATE_PRESSED);
    
    lv_obj_add_event_cb(keyboard, event_handler, LV_EVENT_ALL, this);
    
    visible = true;
}

void UIKeyboard::linkTo(UITextBox* textbox) {
    linkedTextBox = textbox;
    if (keyboard && textbox && textbox->getObject()) {
        lv_keyboard_set_textarea(keyboard, textbox->getObject());
        lv_obj_add_state(textbox->getObject(), LV_STATE_FOCUSED);
    }
}

void UIKeyboard::event_handler(lv_event_t* e) {
    UIKeyboard* instance = (UIKeyboard*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_READY) {
        Serial.println("Keyboard: Enter pressed");
        if (instance && instance->linkedTextBox) {
            String text = instance->linkedTextBox->getText();
            Serial.printf("Keyboard: Text from textbox: %s\n", text.c_str());
            if (instance->enterCallback) {
                instance->enterCallback(text);
            }
            // Use the public submitText() method instead of private triggerSubmit()
            instance->linkedTextBox->submitText();
        }
        if (instance) {
            instance->hide();
        }
    }
    else if (code == LV_EVENT_CANCEL) {
        Serial.println("Keyboard: Cancel pressed");
        if (instance && instance->cancelCallback) {
            instance->cancelCallback();
        }
        if (instance) {
            instance->hide();
        }
    }
}

void UIKeyboard::hide() {
    if (keyboard) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        visible = false;
    }
}

bool UIKeyboard::isVisible() {
    return visible && keyboard && !lv_obj_has_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

void UIKeyboard::onEnter(std::function<void(String)> callback) {
    enterCallback = callback;
}

void UIKeyboard::onCancel(std::function<void()> callback) {
    cancelCallback = callback;
}

void UIKeyboard::setButtonSize(int min_width, int min_height) {
    if (keyboard) {
        lv_obj_set_style_min_height(keyboard, min_height, LV_PART_ITEMS);
        lv_obj_set_style_min_width(keyboard, min_width, LV_PART_ITEMS);
    }
}

#endif // SIMPLEUI_H
