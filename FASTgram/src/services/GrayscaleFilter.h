#pragma once

#include "Filter.h"

class GrayscaleFilter : public Filter {
public:
    std::string getName() const override;
    void apply(ImageBuffer& buffer) const override;
};