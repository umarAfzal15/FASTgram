#include "Avatar.h"
#include <cstring>

Avatar::Avatar(Rectangle bounds, const std::string& initials, Color bgColor)
    : Widget(bounds),
    texture{ 0 },
    hasTexture(false),
    initials(initials),
    bgColor(bgColor),
    borderColor({ 210, 215, 222, 255 }),
    borderThickness(2.0f) {
}

void Avatar::update() {

}
void Avatar::draw() const {
    float radius = bounds.width/2.0f;
    Vector2 center = { bounds.x + radius, bounds.y + radius };

    //Draw the outer border ring
    if (borderThickness > 0.0f) {
        DrawCircleV(center, radius + borderThickness, borderColor);
    }

    //Draw the profile photo OR the initials fallback
    if (hasTexture && texture.id > 0) {
        Rectangle source = {
            0.0f,
            0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };

        DrawTexturePro(texture, source, bounds, { 0.0f, 0.0f }, 0.0f, WHITE);
        DrawRing(center, radius, radius + borderThickness + 2.0f, 0.0f, 360.0f, 36, bgColor);
    }
    else {
        // solid colored circle
        DrawCircleV(center, radius, bgColor);

        int fontSize = static_cast<int>(bounds.height * 0.42f);
        int textWidth = MeasureText(initials.c_str(), fontSize);

        int textX = static_cast<int>(center.x - (textWidth / 2.0f));
        int textY = static_cast<int>(center.y - (fontSize / 2.0f));

        DrawText(initials.c_str(), textX, textY, fontSize, WHITE);
    }
}

bool Avatar::loadTexture(const std::string& imagePath) {
    unloadTexture();

    if (imagePath.empty()) {
        return false;
    }

    Texture2D loaded = LoadTexture(imagePath.c_str());
    if (loaded.id == 0) {
        return false;
    }

    texture = loaded;
    hasTexture = true;
    return true;
}

void Avatar::unloadTexture() {
    if (hasTexture && texture.id > 0) {
        UnloadTexture(texture);
    }
    texture = { 0 };
    hasTexture = false;
}

void Avatar::setInitials(const std::string& text) {
    initials = text;
}
void Avatar::setBorder(float thickness, Color color) {
    borderThickness = thickness;
    borderColor = color;
}

Avatar::~Avatar() {
    unloadTexture();
}