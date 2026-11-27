#pragma once
#include "Screen.h"
#include <vector>

class ScreenManager {
private:
    Screen* currentScreen;
    std::vector<Screen*> screenStack;
    std::vector<Screen*> pendingDelete;

    void flushPendingDeletes();

public:
    ScreenManager();
    ~ScreenManager();

    void setScreen(Screen* newScreen);
    void pushScreen(Screen* newScreen);
    void popScreen();
    void clearStack();

    void update();
    void draw() const;

    bool hasScreen() const;
    bool canGoBack() const;
};