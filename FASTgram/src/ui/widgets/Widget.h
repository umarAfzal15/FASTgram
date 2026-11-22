#pragma once
#include "raylib.h"

class Widget {
protected:
    Rectangle bounds;
    bool visible;
    bool enabled;
    bool hovered;

public:
    Widget(Rectangle bounds = { 0, 0, 0, 0 });
    virtual ~Widget() {};

    // Pure virtual interface (every child widget must implement these)
    virtual void update() = 0;
    virtual void draw() const = 0;

    // Shared hover detection helper
    void updateHover();

    // Position and size setters
    void setPosition(float x, float y);
    void setBounds(Rectangle rect);
    Rectangle getBounds() const;

    // State getters and setters
    void setVisible(bool visible);
    bool isVisible() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    bool isHovered() const;
};