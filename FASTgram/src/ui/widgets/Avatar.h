#pragma once
#include "Widget.h"
#include "raylib.h"
#include <string>

class Avatar : public Widget {
private:
    Texture2D texture;
    bool hasTexture;
    std::string initials;
    Color bgColor;
    Color borderColor;
    float borderThickness;

public:
    Avatar(Rectangle bounds, const std::string& initials = "U", Color bgColor = { 70, 130, 180, 255 });
    ~Avatar();

    void update() override;
    void draw() const override;

    bool loadTexture(const std::string& imagePath);
    void unloadTexture();

    void setInitials(const std::string& text);
    void setBorder(float thickness, Color color);
};