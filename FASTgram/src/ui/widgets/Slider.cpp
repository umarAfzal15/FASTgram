#include "Slider.h"
#include "../src/core/Theme.h"

Slider::Slider(Rectangle bounds, float initialValue) : Widget(bounds), value(clampValue(initialValue)), dragging(false) 
{}

float Slider::clampValue(float v) const {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

void Slider::updateValueFromMouse() {
    if (bounds.width <= 0.0f) return;
    float mouseX = (float)GetMouseX();
    float ratio = (mouseX - bounds.x) / bounds.width;
    value = clampValue(ratio);
}

void Slider::update() {
    if (!visible || !enabled) {
        hovered = false;
        dragging = false;
        return;
    }

    updateHover();

    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        dragging = true;
        updateValueFromMouse();
    }

    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            updateValueFromMouse();
        }
        else {
            dragging = false;
        }
    }
}

void Slider::draw() const {
    if (!visible) return;

    float trackHeight = 6.0f;
    float trackY = bounds.y + (bounds.height - trackHeight) / 2.0f;

    Rectangle fullTrack = { bounds.x, trackY, bounds.width, trackHeight };
    Rectangle filledTrack = { bounds.x, trackY, bounds.width * value, trackHeight };

    Color bgTrackColor = Theme::surface();
    Color fillColor = (hovered || dragging) ? Theme::accentHover() : Theme::accent();
    Color thumbColor = WHITE;
    Color thumbBorder = Theme::accent();

    if (!enabled) {
        bgTrackColor = Fade(bgTrackColor, 0.45f);
        fillColor = Fade(fillColor, 0.45f);
        thumbColor = Fade(thumbColor, 0.6f);
        thumbBorder = Fade(thumbBorder, 0.45f);
    }

    DrawRectangleRounded(fullTrack, 1.0f, 8, bgTrackColor);
    DrawRectangleRoundedLinesEx(fullTrack, 1.0f, 8, 1.0f, Theme::border());

    if (filledTrack.width > 0.0f) {
        DrawRectangleRounded(filledTrack, 1.0f, 8, fillColor);
    }

    float thumbRadius = bounds.height * 0.42f;
    if (thumbRadius < 7.0f) thumbRadius = 7.0f;

    float thumbX = bounds.x + bounds.width * value;
    float thumbY = bounds.y + bounds.height / 2.0f;

    DrawCircle((int)thumbX, (int)thumbY, thumbRadius + 1.5f, thumbBorder);
    DrawCircle((int)thumbX, (int)thumbY, thumbRadius, thumbColor);
}

float Slider::getValue() const {
    return value;
}

void Slider::setValue(float v) {
    value = clampValue(v);
}