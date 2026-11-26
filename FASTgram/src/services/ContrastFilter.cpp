#include "ContrastFilter.h"

ContrastFilter::ContrastFilter(float initialStrength) {
    setStrength(initialStrength);
}

std::string ContrastFilter::getName() const {
    return "Contrast";
}

void ContrastFilter::setStrength(float newStrength) {
    // Clamp between -1.0f (lowest contrast) and +1.0f (highest contrast)
    if (newStrength < -1.0f) newStrength = -1.0f;
    if (newStrength > 1.0f)  newStrength = 1.0f;
    strength = newStrength;
}

float ContrastFilter::getStrength() const {
    return strength;
}

void ContrastFilter::apply(ImageBuffer& buffer) const {
    if (!buffer.isValid()) {
        return;
    }

    // Raylib's ImageColorContrast expects a float from -100.0f to +100.0f
    float contrastValue = strength * 100.0f;

    Image& img = buffer.getRawImage();
    ImageColorContrast(&img, contrastValue);
}