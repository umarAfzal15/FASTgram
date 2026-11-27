#include "SettingsScreen.h"
#include "../src/core/Theme.h"

SettingsScreen::SettingsScreen(Rectangle contentBounds, const std::string& username)
    : bounds(contentBounds),
    currentUsername(username),
    backBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    themeRowBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    windowSmallBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    windowLargeBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    accountRowBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    logoutBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    darkModeToggle({ 0.0f, 0.0f, 50.0f, 28.0f }, Theme::isDark()),
    backRequested(false),
    logoutRequested(false)
{
    updateLayout();
}

void SettingsScreen::setBounds(Rectangle contentBounds) {
    bounds = contentBounds;
    updateLayout();
}

bool SettingsScreen::isBackRequested() {
    if (backRequested) {
        backRequested = false;
        return true;
    }
    return false;
}

bool SettingsScreen::isLogoutRequested() {
    if (logoutRequested) {
        logoutRequested = false;
        return true;
    }
    return false;
}

void SettingsScreen::updateLayout() {
    float pad = 16.0f;
    float rowW = bounds.width - (pad * 2.0f);
    float rowH = 54.0f;
    float x = bounds.x + pad;
    float y = bounds.y + 16.0f;

    backBtnBounds = { x, y, 74.0f, 34.0f };
    y += 50.0f;

    themeRowBounds = { x, y, rowW, rowH };
    darkModeToggle.setBounds({
        themeRowBounds.x + themeRowBounds.width - 66.0f,
        themeRowBounds.y + (themeRowBounds.height - 28.0f) / 2.0f,
        50.0f,
        28.0f
        });
    y += rowH + 12.0f;

    float halfW = (rowW - 10.0f) / 2.0f;
    windowSmallBtnBounds = { x, y + 26.0f, halfW, 36.0f };
    windowLargeBtnBounds = { x + halfW + 10.0f, y + 26.0f, halfW, 36.0f };
    y += 74.0f;

    accountRowBounds = { x, y, rowW, rowH };

    logoutBtnBounds = { x, bounds.y + bounds.height - 52.0f - pad, rowW, 48.0f };
}

void SettingsScreen::update() {
    updateLayout();

    if (logoutModal.isOpen()) {
        logoutModal.update();
        if (logoutModal.isConfirmed()) {
            Logger::log("Logout: " + currentUsername);
            logoutRequested = true;
        }
        return;
    }

    darkModeToggle.setToggled(Theme::isDark());
    darkModeToggle.update();
    if (darkModeToggle.isToggled() != Theme::isDark()) {
        Theme::toggle();
        Theme::saveConfig();
        Logger::log(Theme::isDark() ? "Theme set to Dark" : "Theme set to Light");
        return;
    }

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, backBtnBounds)) {
            backRequested = true;
        }
        else if (CheckCollisionPointRec(mouse, themeRowBounds)) {
            Theme::toggle();
            darkModeToggle.setToggled(Theme::isDark());
            Theme::saveConfig();
            Logger::log(Theme::isDark() ? "Theme set to Dark" : "Theme set to Light");
        }
        else if (CheckCollisionPointRec(mouse, windowSmallBtnBounds)) {
            Theme::setWindowSize(480, 800);
            SetWindowSize(480, 800);
            Theme::saveConfig();
            Logger::log("Window resized to 480x800");
        }
        else if (CheckCollisionPointRec(mouse, windowLargeBtnBounds)) {
            Theme::setWindowSize(960, 840);
            SetWindowSize(960, 840);
            Theme::saveConfig();
            Logger::log("Window resized to 960x840");
        }
        else if (CheckCollisionPointRec(mouse, logoutBtnBounds)) {
            logoutModal.open("Log Out", "Are you sure you want to log out?", "Log Out", "Cancel");
        }
    }
}

void SettingsScreen::draw() const {
    DrawRectangleRec(bounds, Theme::background());

    Vector2 mouse = GetMousePosition();

    Color backBg = CheckCollisionPointRec(mouse, backBtnBounds) ? Theme::surfaceHover() : Theme::surface();
    DrawRectangleRounded(backBtnBounds, 0.25f, 8, backBg);
    DrawRectangleRoundedLinesEx(backBtnBounds, 0.25f, 8, 1.0f, Theme::border());
    DrawTextEx(Theme::appFont, "< Back", { backBtnBounds.x + 12.0f, backBtnBounds.y + 8.0f }, 15.0f, 1.0f, Theme::primaryText());

    DrawTextEx(Theme::appFont, "Settings", { backBtnBounds.x + 88.0f, backBtnBounds.y + 5.0f }, 22.0f, 1.0f, Theme::primaryText());

    Color themeBg = CheckCollisionPointRec(mouse, themeRowBounds) ? Theme::surfaceHover() : Theme::surface();
    DrawRectangleRounded(themeRowBounds, 0.18f, 8, themeBg);
    DrawRectangleRoundedLinesEx(themeRowBounds, 0.18f, 8, 1.0f, Theme::border());
    DrawTextEx(Theme::appFont, "Dark Mode", { themeRowBounds.x + 16.0f, themeRowBounds.y + 17.0f }, 17.0f, 1.0f, Theme::primaryText());
    darkModeToggle.draw();

    DrawTextEx(Theme::appFont, "Window Size", { windowSmallBtnBounds.x, windowSmallBtnBounds.y - 24.0f }, 15.0f, 1.0f, Theme::secondaryText());

    bool isCompact = (Theme::getWindowWidth() == 480);
    const char* smallLabel = (windowSmallBtnBounds.width < 185.0f) ? "Compact (480)" : "Compact (480x800)";
    const char* largeLabel = (windowLargeBtnBounds.width < 185.0f) ? "Desktop (960)" : "Desktop (960x840)";
    float btnFontSize = (windowSmallBtnBounds.width < 185.0f) ? 13.5f : 15.0f;

    Color smallBg = isCompact ? Theme::accent() : (CheckCollisionPointRec(mouse, windowSmallBtnBounds) ? Theme::surfaceHover() : Theme::surface());
    Color smallText = isCompact ? (Theme::isDark() ? Theme::background() : WHITE) : Theme::primaryText();
    DrawRectangleRounded(windowSmallBtnBounds, 0.2f, 8, smallBg);
    DrawRectangleRoundedLinesEx(windowSmallBtnBounds, 0.2f, 8, 1.0f, Theme::border());
    Vector2 smallSize = MeasureTextEx(Theme::appFont, smallLabel, btnFontSize, 1.0f);
    DrawTextEx(Theme::appFont, smallLabel, {
        windowSmallBtnBounds.x + (windowSmallBtnBounds.width - smallSize.x) / 2.0f,
        windowSmallBtnBounds.y + (windowSmallBtnBounds.height - smallSize.y) / 2.0f
        }, btnFontSize, 1.0f, smallText);

    Color largeBg = !isCompact ? Theme::accent() : (CheckCollisionPointRec(mouse, windowLargeBtnBounds) ? Theme::surfaceHover() : Theme::surface());
    Color largeText = !isCompact ? (Theme::isDark() ? Theme::background() : WHITE) : Theme::primaryText();
    DrawRectangleRounded(windowLargeBtnBounds, 0.2f, 8, largeBg);
    DrawRectangleRoundedLinesEx(windowLargeBtnBounds, 0.2f, 8, 1.0f, Theme::border());
    Vector2 largeSize = MeasureTextEx(Theme::appFont, largeLabel, btnFontSize, 1.0f);
    DrawTextEx(Theme::appFont, largeLabel, {
        windowLargeBtnBounds.x + (windowLargeBtnBounds.width - largeSize.x) / 2.0f,
        windowLargeBtnBounds.y + (windowLargeBtnBounds.height - largeSize.y) / 2.0f
        }, btnFontSize, 1.0f, largeText);

    DrawRectangleRounded(accountRowBounds, 0.18f, 8, Theme::surface());
    DrawRectangleRoundedLinesEx(accountRowBounds, 0.18f, 8, 1.0f, Theme::border());
    DrawTextEx(Theme::appFont, "Logged in as", { accountRowBounds.x + 16.0f, accountRowBounds.y + 17.0f }, 16.0f, 1.0f, Theme::secondaryText());

    std::string handle = "@" + currentUsername;
    Vector2 handleSize = MeasureTextEx(Theme::appFont, handle.c_str(), 16.0f, 1.0f);
    DrawTextEx(Theme::appFont, handle.c_str(), { accountRowBounds.x + accountRowBounds.width - handleSize.x - 16.0f, accountRowBounds.y + 17.0f }, 16.0f, 1.0f, Theme::primaryText());

    Color logoutBg = CheckCollisionPointRec(mouse, logoutBtnBounds) ? Theme::surfaceHover() : Theme::surface();
    DrawRectangleRounded(logoutBtnBounds, 0.2f, 8, logoutBg);
    DrawRectangleRoundedLinesEx(logoutBtnBounds, 0.2f, 8, 1.5f, Theme::danger());

    Vector2 logoutSize = MeasureTextEx(Theme::appFont, "Log Out", 18.0f, 1.0f);
    Vector2 logoutPos = {
        logoutBtnBounds.x + (logoutBtnBounds.width - logoutSize.x) / 2.0f,
        logoutBtnBounds.y + (logoutBtnBounds.height - logoutSize.y) / 2.0f
    };
    DrawTextEx(Theme::appFont, "Log Out", logoutPos, 18.0f, 1.0f, Theme::danger());

    logoutModal.draw();
}