#pragma once

#include "Filter.h"

class BlurFilter : public Filter {
private:
    float strength; // Controls blur radius (e.g., 1 to 10 pixels)

public:
    BlurFilter(float initialStrength = 2.0f);

    std::string getName() const override;
    void setStrength(float newStrength) override;
    float getStrength() const override;
    void apply(ImageBuffer& buffer) const override;
};