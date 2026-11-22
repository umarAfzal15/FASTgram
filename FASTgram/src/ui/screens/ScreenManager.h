#pragma once
#include "Screen.h"

class ScreenManager {
private:
    Screen* currentScreen;

public:
    ScreenManager();
    ~ScreenManager();

    // Switch to a new screen (cleans up old screen, initializes new one)
    void setScreen(Screen* newScreen);

    // Frame loop forwarding
    void update();
    void draw() const;

    // State check
    bool hasScreen() const;
};