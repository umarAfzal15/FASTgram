#include "Toast.h"
#include "../src/core/Theme.h"

Toast::Toast()
    : Widget({ 0.0f, 0.0f, 0.0f, 0.0f }),
    message(""),
    type(ToastType::Info),
    duration(3.0f),
    timer(0.0f),
    visible(false)
{
}

void Toast::show(const std::string& msg, ToastType toastType, float seconds) {
    message = msg;
    type = toastType;
    duration = seconds;
    timer = 0.0f;
    visible = true;
}

void Toast::update() {
    if (!visible) return;

    timer += GetFrameTime();
    if (timer >= duration) {
        hide();
    }
}

bool Toast::isVisible() const {
    return visible;
}

void Toast::draw() const {
    if (!visible) return;

    float fontSize = 18.0f;
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(Theme::appFont, message.c_str(), fontSize, spacing);

    float paddingX = 24.0f;
    float paddingY = 12.0f;

    float boxWidth = textSize.x + (paddingX * 2.0f);
    float boxHeight = textSize.y + (paddingY * 2.0f);
    float boxX = (GetScreenWidth() - boxWidth) / 2.0f;
    float boxY = GetScreenHeight() - boxHeight - 30.0f;

    Rectangle box = { boxX, boxY, boxWidth, boxHeight };

    Color bgColor = Theme::accent();
    if (type == ToastType::Success) {
        bgColor = Theme::success();
    }
    else if (type == ToastType::Error) {
        bgColor = Theme::danger();
    }

    Color textColor = Theme::isDark() ? Theme::background() : WHITE;

    DrawRectangleRounded(box, 0.3f, 8, bgColor);
    DrawRectangleRoundedLinesEx(box, 0.3f, 8, 1.5f, Theme::border());

    Vector2 textPos = { boxX + paddingX, boxY + paddingY };
    DrawTextEx(Theme::appFont, message.c_str(), textPos, fontSize, spacing, textColor);
}
void Toast::hide() {
    visible = false;
    timer = 0.0f;
}