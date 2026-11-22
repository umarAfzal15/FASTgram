#pragma once
#include <string>
#include "Widget.h"

class TextBox : public Widget {
private:
    std::string text;
    std::string placeholder;
    bool focused;
    bool isPassword;
    int maxLength;
    int fontSize;

public:
    TextBox(Rectangle bounds = { 0, 0, 0, 0 },
        const std::string& placeholder = "",
        bool isPassword = false,
        int maxLength = 50,
        int fontSize = 18);

    void update() override;
    void draw() const override;

    // Text accessors
    std::string getText() const;
    void setText(const std::string& text);
    void clear();

    // Focus & configuration
    bool isFocused() const;
    void setFocused(bool focused);
    void setPlaceholder(const std::string& placeholder);
    void setPasswordMode(bool isPassword);
};