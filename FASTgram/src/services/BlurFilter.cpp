#include "BlurFilter.h"

BlurFilter::BlurFilter(float initialStrength) {
    setStrength(initialStrength);
}

std::string BlurFilter::getName() const {
    return "Blur";
}

void BlurFilter::setStrength(float newStrength) {
    // cap at 10.0f so it stays responsive
    if (newStrength < 0.0f)  newStrength = 0.0f;
    if (newStrength > 10.0f) newStrength = 10.0f;
    strength = newStrength;
}

float BlurFilter::getStrength() const {
    return strength;
}

void BlurFilter::apply(ImageBuffer& buffer) const {
    if (!buffer.isValid()) {
        return;
    }

    int blurSize = static_cast<int>(strength);
    if (blurSize <= 0) {
        return; // Nothing to blur at strength 0
    }

    Image& img = buffer.getRawImage();
    ImageBlurGaussian(&img, blurSize);
}