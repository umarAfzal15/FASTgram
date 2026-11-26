#include "BrightnessFilter.h"

BrightnessFilter::BrightnessFilter(float initialStrength) {
    setStrength(initialStrength);
}

std::string BrightnessFilter::getName() const {
    return "Brightness";
}

void BrightnessFilter::setStrength(float newStrength) {
    if (newStrength < -1.0f) newStrength = -1.0f;
    if (newStrength > 1.0f)  newStrength = 1.0f;
    strength = newStrength;
}

float BrightnessFilter::getStrength() const {
    return strength;
}

void BrightnessFilter::apply(ImageBuffer& buffer) const {
    if (!buffer.isValid()) {
        return;
    }

    // Raylib expects brightness as an integer from -255 to +255
    int brightnessValue = static_cast<int>(strength * 255.0f);

    Image& img = buffer.getRawImage();
    ImageColorBrightness(&img, brightnessValue);
}