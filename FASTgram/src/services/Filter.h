#pragma once

#include <string>
#include "ImageStore.h"

class Filter {
public:
    virtual ~Filter() = default;

    // Human-readable label for UI filter buttons
    virtual std::string getName() const = 0;

    // Adjustable slider strength (default 1.0f; overridden by adjustable filters)
    virtual void setStrength(float strength) {}
    virtual float getStrength() const { return 1.0f; }

    // Core polymorphic pixel operation on ImageBuffer
    virtual void apply(ImageBuffer& buffer) const = 0;

    // Functor call operator so any Filter can be invoked as filter(buffer)
    void operator()(ImageBuffer& buffer) const {
        apply(buffer);
    }
};