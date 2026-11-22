#pragma once

class Screen {
public:
    virtual ~Screen() = default;

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void update() = 0;
    virtual void draw() const = 0;
};