#pragma once

#include "Filter.h"

class ContrastFilter : public Filter {
private:
    float strength; // e.g., -1.0f to +1.0f (or multiplier around 1.0f)

public:
    ContrastFilter(float initialStrength = 0.25f);

    std::string getName() const override;
    void setStrength(float newStrength) override;
    float getStrength() const override;
    void apply(ImageBuffer& buffer) const override;
};