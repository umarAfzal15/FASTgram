#pragma once
#include "Widget.h"

class Slider : public Widget {
private:
    float value;    
    bool dragging;

    float clampValue(float v) const;
    void updateValueFromMouse();

public:
    Slider(Rectangle bounds = { 0, 0, 200, 24 }, float initialValue = 0.5f);

    void update() override;
    void draw() const override;

    float getValue() const;
    void setValue(float v);
};