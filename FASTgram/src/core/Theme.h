#pragma once
#include "raylib.h"

class Theme {
private:
    static bool darkMode;

public:
    static Font appFont;
    static void toggle();
    static void setDark(bool dark);
    static bool isDark();

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