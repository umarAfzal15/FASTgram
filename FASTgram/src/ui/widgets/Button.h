#pragma once
#include <string>
#include "Widget.h"

enum class ButtonStyle {
    Primary,    // Main colored CTA ("Log in", "Share")
    Secondary,  // Neutral surface button ("Cancel", "Edit Profile")
    Danger      // Destructive action ("Delete", "Log Out")
};

class Button : public Widget {
private:
    std::string text;
    ButtonStyle style;
    int fontSize;
    bool clicked;

public:
    Button(Rectangle bounds = { 0, 0, 0, 0 },
        const std::string& text = "",
        ButtonStyle style = ButtonStyle::Primary,
        int fontSize = 18);

    void update() override;
    void draw() const override;

    // Click query
    bool isClicked() const;

    // Text and style setters
    void setText(const std::string& text);
    std::string getText() const;
    void setStyle(ButtonStyle style);
};