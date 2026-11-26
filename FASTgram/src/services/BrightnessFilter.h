#pragma once

#include "Filter.h"

class BrightnessFilter : public Filter {
private:
    float strength; // e.g., -1.0f (darker) to +1.0f (brighter)

public:
    BrightnessFilter(float initialStrength = 0.25f);

    std::string getName() const override;
    void setStrength(float newStrength) override;
    float getStrength() const override;
    void apply(ImageBuffer& buffer) const override;
};