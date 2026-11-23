#include "Button.h"
#include <string>
#include "../src/core/Theme.h"
using namespace std;

Button::Button(Rectangle bounds, const string& text, ButtonStyle style, int fontSize): Widget(bounds) {
	this->bounds = bounds;
	this->text = text;
	this->style = style;
	this->fontSize = fontSize;
    clicked = false;
}

void Button::update() {

	clicked = false;

	if (!visible || !enabled) {
		hovered = false;
		return;
	}

	updateHover();

	if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		clicked = true;
	}
}

void Button::draw() const {
    if (!visible) return;

    Color bgColor = Theme::accent();
    Color textColor = Theme::primaryText();

    if (style == ButtonStyle::Primary) {
        bgColor = hovered ? Theme::accentHover() : Theme::accent();
        textColor = WHITE;
    }
    else if (style == ButtonStyle::Secondary) {
        bgColor = Theme::surface(); // Or Theme::border() when hovered
        textColor = Theme::primaryText();
    }
    else if (style == ButtonStyle::Danger) {
        bgColor = Theme::danger();
        textColor = WHITE;
    }

    // Dim the button if it is disabled
    if (!enabled) {
        bgColor = Fade(bgColor, 0.45f);
        textColor = Fade(textColor, 0.6f);
    }

    DrawRectangleRounded(bounds, 0.25f, 8, bgColor);

    if (style == ButtonStyle::Secondary) {
        DrawRectangleRoundedLinesEx(bounds, 0.25f, 8, 1.5f, Theme::border());
    }

    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(Theme::appFont, text.c_str(), (float)fontSize, spacing);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2.0f,
        bounds.y + (bounds.height - textSize.y) / 2.0f
    };

    DrawTextEx(Theme::appFont, text.c_str(), textPos, (float)fontSize, spacing, textColor);
}

// Click query
bool Button::isClicked() const {
    return clicked;
}

void Button::setText(const std::string& text) {
    this->text = text;
}

std::string Button::getText() const {
    return this->text;
}

void Button::setStyle(ButtonStyle style) {
    this->style = style;
}