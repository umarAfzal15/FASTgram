#include "ScrollView.h"

ScrollView::ScrollView(Rectangle bounds, float scrollSpeed)
    : Widget(bounds),
    scrollOffset(0.0f),
    contentHeight(0.0f),
    scrollSpeed(scrollSpeed)
{
}

void ScrollView::setContentHeight(float height) {
    this->contentHeight = height;

    float maxScroll = this->contentHeight - this->bounds.height;
    if (maxScroll < 0.0f) {
        maxScroll = 0.0f;
    }

    if (this->scrollOffset > maxScroll) {
        this->scrollOffset = maxScroll;
    }
}

float ScrollView::getScrollOffset() const {
    return scrollOffset;
}

void ScrollView::resetScroll() {
    scrollOffset = 0.0f;
}

void ScrollView::update() {
    float mouseMove = GetMouseWheelMove();
    float x = static_cast<float>(GetMouseX());
    float y = static_cast<float>(GetMouseY());

    bool isDownKeyHeld = IsKeyDown(KEY_DOWN);
    bool isUpKeyHeld = IsKeyDown(KEY_UP);

    bool isInBound = (x >= bounds.x && x <= bounds.width + bounds.x) && (y >= bounds.y && y <= bounds.height + bounds.y);

    if ((mouseMove != 0.0f || isDownKeyHeld || isUpKeyHeld) && isInBound) {

        if (mouseMove != 0.0f) {
            scrollOffset -= mouseMove * scrollSpeed;
        }

        float keyScrollSpeed = 400.0f;
        if (isDownKeyHeld) {
            scrollOffset += keyScrollSpeed * GetFrameTime();
        }
        if (isUpKeyHeld) {
            scrollOffset -= keyScrollSpeed * GetFrameTime();
        }

        float maxScroll = contentHeight - bounds.height;
        if (maxScroll < 0.0f) {
            maxScroll = 0.0f;
        }

        if (scrollOffset < 0.0f) {
            scrollOffset = 0.0f;
        }
        else if (scrollOffset > maxScroll) {
            scrollOffset = maxScroll;
        }
    }
}

void ScrollView::draw() const {
    
    if (contentHeight <= bounds.height) {
        return;
    }

    float barWidth = 6.0f;
    float padding = 4.0f;
    float trackX = bounds.x + bounds.width - barWidth - padding;
    float trackY = bounds.y + padding;
    float trackHeight = bounds.height - (padding * 2.0f);

    Rectangle track = { trackX, trackY, barWidth, trackHeight };
    DrawRectangleRounded(track, 1.0f, 6, { 220, 224, 230, 120 }); //220, 224, 230 , 120 are RGB values

    float visibleRatio = bounds.height / contentHeight;
    float thumbHeight = trackHeight * visibleRatio;
    if (thumbHeight < 24.0f) {
        thumbHeight = 24.0f; // Keep thumb big enough to see on very long feeds
    }

    float maxScroll = contentHeight - bounds.height;
    float scrollRatio = scrollOffset / maxScroll;
    float availableTravel = trackHeight - thumbHeight;
    float thumbY = trackY + (scrollRatio * availableTravel);

    Rectangle thumb = { trackX, thumbY, barWidth, thumbHeight };
    DrawRectangleRounded(thumb, 1.0f, 6, { 140, 150, 165, 220 });
}

void ScrollView::beginClip() const {
    BeginScissorMode(
        static_cast<int>(bounds.x),
        static_cast<int>(bounds.y),
        static_cast<int>(bounds.width),
        static_cast<int>(bounds.height)
    );
}

void ScrollView::endClip() const {
    EndScissorMode();
}
