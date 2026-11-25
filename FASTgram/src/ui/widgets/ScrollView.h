#pragma once
#include "raylib.h"
#include "Widget.h"

class ScrollView : public Widget {
private:
    float scrollOffset;
    float contentHeight;
    float scrollSpeed;

public:
    ScrollView(Rectangle bounds, float scrollSpeed = 35.0f);

    void setContentHeight(float height);
    float getScrollOffset() const;
    void resetScroll();

    void update() override;
    void draw() const override;

    // Call beginClip() before drawing child items offset by -scrollOffset, then endClip() after
    void beginClip() const;
    void endClip() const;
};