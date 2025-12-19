#include "SimpleUI.h"

// ==================== Global Functions ====================
void SimpleUI_init() {
    // Placeholder for future initialization if needed
    // Currently does nothing - user handles LVGL tick manually for compatibility
    // with existing code patterns (see example's loop() function)
}

void SimpleUI_update() {
    lv_task_handler();
}

// ==================== Screen Implementation ====================
UIScreen::UIScreen() : screen(nullptr) {
    // Don't create LVGL objects in constructor - do it in load()
}

UIScreen::~UIScreen() {
    if (screen && lv_obj_is_valid(screen)) {
        lv_obj_del(screen);
    }
}

void UIScreen::load() {
    // Create screen if it doesn't exist
    if (!screen) {
        screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(screen, lv_color_hex(0x1a1a1a), 0);
        // Disable scrolling on main screen
        lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    }
    lv_scr_load(screen);
}

void UIScreen::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
    // Create screen if it doesn't exist
    if (!screen) {
        screen = lv_obj_create(NULL);
    }
    uint32_t color = (r << 16) | (g << 8) | b;
    lv_obj_set_style_bg_color(screen, lv_color_hex(color), 0);
}

// ==================== Label Implementation ====================
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

// ==================== Button Implementation ====================
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
    
    // Style
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_pad_all(btn, 10, 0);
    
    // Label
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

// ==================== TextBox Implementation ====================
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
    
    // Style
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0x2a2a2a), 0);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x555555), 0);
    lv_obj_set_style_border_width(textarea, 2, 0);
    lv_obj_set_style_text_color(textarea, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_pad_all(textarea, 8, 0);
    
    // Focused style
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x2196F3), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(textarea, 3, LV_STATE_FOCUSED);
    
    // Register events
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
            instance->triggerSubmit();
        }
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

void UITextBox::triggerSubmit() {
    if (submitCallback) {
        submitCallback(getText());
    }
}

// ==================== Keyboard Implementation ====================
UIKeyboard::UIKeyboard() : keyboard(nullptr), linkedTextBox(nullptr), visible(false) {}

UIKeyboard::~UIKeyboard() {
    if (keyboard && lv_obj_is_valid(keyboard)) {
        lv_obj_del(keyboard);
    }
}

void UIKeyboard::spawn(lv_obj_t* parent, int x, int y, int w, int h) {
    if (!parent) parent = lv_scr_act();
    
    // Delete existing keyboard if valid
    if (keyboard && lv_obj_is_valid(keyboard)) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }
    
    keyboard = lv_keyboard_create(parent);
    
    // Get parent dimensions
    int parent_w = lv_obj_get_width(parent);
    int parent_h = lv_obj_get_height(parent);
    
    // Use full screen width (edge to edge) and half screen height
    // Let the keyboard extend to the edges on left and right
    int keyboard_w = parent_w;  // Full width
    int keyboard_h = parent_h / 2;  // Half screen height
    
    lv_obj_set_size(keyboard, keyboard_w, keyboard_h);
    
    // Position at bottom, flush with both edges (center horizontally)
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Remove all padding around the keyboard to use full width
    lv_obj_set_style_pad_all(keyboard, 0, 0);
    lv_obj_set_style_pad_gap(keyboard, 2, 0);  // Reduced gap for bigger buttons
    
    // Main keyboard background style - extend to edges
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_width(keyboard, 0, 0);
    lv_obj_set_style_radius(keyboard, 0, 0);  // No rounded corners at bottom
    
    // Make the keyboard buttons much bigger - fill the space
    // Bigger padding for bigger touch targets
    lv_obj_set_style_pad_all(keyboard, 15, LV_PART_ITEMS);  // Increased padding
    lv_obj_set_style_pad_gap(keyboard, 2, LV_PART_ITEMS);   // Minimal gap between buttons
    lv_obj_set_style_min_height(keyboard, (keyboard_h / 4) - 10, LV_PART_ITEMS);  // Dynamic min height based on keyboard height
    lv_obj_set_style_min_width(keyboard, (keyboard_w / 10) - 5, LV_PART_ITEMS);   // Dynamic min width
    
    // Button colors
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x333333), LV_PART_ITEMS);
    lv_obj_set_style_radius(keyboard, 8, LV_PART_ITEMS);
    lv_obj_set_style_text_color(keyboard, lv_color_hex(0xffffff), LV_PART_ITEMS);
    
    // Larger text for better readability on bigger buttons
    lv_obj_set_style_text_font(keyboard, &lv_font_montserrat_20, LV_PART_ITEMS);
    
    // Pressed button style
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x2196F3), LV_PART_ITEMS | LV_STATE_PRESSED);
    
    // Make sure it's on top
    lv_obj_move_foreground(keyboard);
    
    // Force a layout update to ensure buttons fill the space
    lv_obj_update_layout(keyboard);
    
    lv_obj_add_event_cb(keyboard, event_handler, LV_EVENT_ALL, this);
    
    visible = true;
}

void UIKeyboard::linkTo(UITextBox* textbox) {
    linkedTextBox = textbox;
    if (keyboard && textbox && textbox->textarea) {
        lv_keyboard_set_textarea(keyboard, textbox->textarea);
        // Focus the textarea when keyboard is linked
        lv_obj_add_state(textbox->textarea, LV_STATE_FOCUSED);
    }
}

void UIKeyboard::event_handler(lv_event_t* e) {
    UIKeyboard* instance = (UIKeyboard*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_READY) {
        // Enter key pressed
        Serial.println("Keyboard: Enter pressed");
        if (instance && instance->linkedTextBox) {
            String text = instance->linkedTextBox->getText();
            Serial.printf("Keyboard: Text from textbox: %s\n", text.c_str());
            if (instance->enterCallback) {
                instance->enterCallback(text);
            }
            instance->linkedTextBox->triggerSubmit();
        }
        if (instance) {
            instance->hide();
        }
    }
    else if (code == LV_EVENT_CANCEL) {
        // Cancel/close button pressed
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
