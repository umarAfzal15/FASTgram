#include "TextBox.h"
#include <string>
#include "src/core/Theme.h"

TextBox::TextBox(Rectangle bounds, const std::string& placeholder, bool isPassword, int maxLength, int fontSize)
    : Widget(bounds) {
    this->text = "";
    this->placeholder = placeholder;
    this->focused = false;
    this->isPassword = isPassword;
    this->maxLength = maxLength;
    this->fontSize = fontSize;
}

void TextBox::update() {
    if (!visible || !enabled) {
        hovered = false;
        focused = false;
        return;
    }

    updateHover();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        focused = hovered;
    }

    if (focused) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32 && key <= 125) && (int)text.length() < maxLength) {
                text += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !text.empty()) {
            text.pop_back();
        }
    }
}

void TextBox::draw() const {
    if (!visible) return;

    // 1. Background and border colors
    Color bgColor = Theme::surface();
    Color borderColor = focused ? Theme::accent() : Theme::border();

    if (!enabled) {
        bgColor = Fade(bgColor, 0.5f);
        borderColor = Fade(borderColor, 0.5f);
    }

    DrawRectangleRounded(bounds, 0.2f, 8, bgColor);
    DrawRectangleRoundedLinesEx(bounds, 0.2f, 8, focused ? 2.0f : 1.5f, borderColor);

    // 2. Decide whether to show placeholder or typed/masked text
    int padding = 12;
    int textX = static_cast<int>(bounds.x) + padding;
    int textY = static_cast<int>(bounds.y + (bounds.height - fontSize) / 2);

    std::string displayStr;
    Color textColor;

    if (text.empty() && !focused) {
        displayStr = placeholder;
        textColor = Theme::secondaryText();
    }
    else {
        displayStr = isPassword ? std::string(text.length(), '*') : text;
        textColor = enabled ? Theme::primaryText() : Fade(Theme::primaryText(), 0.5f);
    }

    DrawText(displayStr.c_str(), textX, textY, fontSize, textColor);

    // 3. Draw blinking cursor when focused
    if (focused && enabled) {
        int blink = static_cast<int>(GetTime() * 2.0) % 2;
        if (blink == 0) {
            int cursorX = textX + MeasureText(displayStr.c_str(), fontSize) + 2;
            int cursorTop = static_cast<int>(bounds.y) + 8;
            int cursorBottom = static_cast<int>(bounds.y + bounds.height) - 8;
            DrawLine(cursorX, cursorTop, cursorX, cursorBottom, Theme::primaryText());
        }
    }
}

// Text accessors
std::string TextBox::getText() const {
    return text;
}

void TextBox::setText(const std::string& text) {
    this->text = text.substr(0, maxLength);
}

void TextBox::clear() {
    text.clear();
}

// Focus & configuration
bool TextBox::isFocused() const {
    return focused;
}

void TextBox::setFocused(bool focused) {
    this->focused = focused;
}

void TextBox::setPlaceholder(const std::string& placeholder) {
    this->placeholder = placeholder;
}

void TextBox::setPasswordMode(bool isPassword) {
    this->isPassword = isPassword;
}