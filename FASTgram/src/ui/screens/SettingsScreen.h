#pragma once
#include "Screen.h"
#include "../src/ui/Widgets/Modal.h"
#include "../src/core/Theme.h"
#include "../src/core/Logger.h"
#include "raylib.h"
#include <string>

class SettingsScreen : public Screen {
private:
    Rectangle bounds;
    std::string currentUsername;

    Rectangle backBtnBounds;
    Rectangle themeRowBounds;
    Rectangle windowSmallBtnBounds;
    Rectangle windowLargeBtnBounds;
    Rectangle accountRowBounds;
    Rectangle logoutBtnBounds;

    Modal logoutModal;
    bool backRequested;
    bool logoutRequested;

    void updateLayout();

public:
    SettingsScreen(Rectangle contentBounds, const std::string& username);

    void setBounds(Rectangle contentBounds);
    bool isBackRequested();
    bool isLogoutRequested();

    void update() override;
    void draw() const override;
};