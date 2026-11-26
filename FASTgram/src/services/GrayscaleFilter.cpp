#include "GrayscaleFilter.h"

std::string GrayscaleFilter::getName() const {
    return "Grayscale";
}

void GrayscaleFilter::apply(ImageBuffer& buffer) const {
    if (!buffer.isValid()) {
        return;
    }

    Image& img = buffer.getRawImage();
    ImageColorGrayscale(&img);
}