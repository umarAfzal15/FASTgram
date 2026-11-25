#pragma once
#include "raylib.h"
#include <string>

class Theme {
private:
    static bool darkMode;
    static int windowWidth;
    static int windowHeight;

public:
    static Font appFont;
    static void toggle();
    static void setDark(bool dark);
    static bool isDark();

    static void setWindowSize(int width, int height);
    static int getWindowWidth();
    static int getWindowHeight();

    static void loadConfig(const std::string& filePath = "data/config.txt");
    static void saveConfig(const std::string& filePath = "data/config.txt");
    static void loadConfig(int& w, int& h);
    static void saveConfig(int w, int h);

    // Core UI Palette
    static Color outerBackground(); // Window backdrop outside the 480px column
    static Color background();      // Main 480px column page surface
    static Color surface();         // Cards, text boxes, and post containers
    static Color surfaceHover();    // Hovered buttons or list items
    static Color primaryText();     // Headings, usernames, main body text
    static Color secondaryText();   // Placeholders, timestamps, subtitles
    static Color border();          // 1px dividers and input box outlines
    static Color accent();          // Primary buttons ("Log in", "Sign up")
    static Color accentHover();     // Primary button hover state
    static Color danger();          // Error messages, delete, unlike heart
    static Color success();         // Validated roll number, online dot
};