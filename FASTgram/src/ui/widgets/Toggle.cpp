#include "Toggle.h"
#include "../src/core/Theme.h"

Toggle::Toggle(Rectangle bounds, bool initialState) : Widget(bounds), toggled(initialState) 
{}

void Toggle::update() {
    if (!visible || !enabled) {
        hovered = false;
        return;
    }

    updateHover();

    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        toggled = !toggled;
    }
}

void Toggle::draw() const {
    if (!visible) return;

    Color trackColor = toggled
        ? (hovered ? Theme::accentHover() : Theme::accent())
        : (hovered ? Theme::surfaceHover() : Theme::surface());

    Color knobColor = WHITE;

    if (!enabled) {
        trackColor = Fade(trackColor, 0.45f);
        knobColor = Fade(knobColor, 0.6f);
    }

    DrawRectangleRounded(bounds, 1.0f, 16, trackColor);

    if (!toggled) {
        DrawRectangleRoundedLinesEx(bounds, 1.0f, 16, 1.5f, Theme::border());
    }

    float padding = 3.0f;
    float radius = (bounds.height - padding * 2.0f) / 2.0f;
    float knobY = bounds.y + bounds.height / 2.0f;
    float knobX = toggled
        ? (bounds.x + bounds.width - padding - radius)
        : (bounds.x + padding + radius);

    DrawCircle((int)knobX, (int)knobY, radius, knobColor);
}

bool Toggle::isToggled() const {
    return toggled;
}

void Toggle::setToggled(bool val) {
    toggled = val;
}