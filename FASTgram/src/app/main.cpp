#include "raylib.h"
#include "src/core/Theme.h"
#include "src/ui/screens/ScreenManager.h"
#include "src/ui/screens/LoginScreen.h"
#include <string>

using namespace std;

int main() {
    // Window dimensions
    const int screenWidth = 480;
    const int screenHeight = 820;

    // Initialize the window and bring it to the screen
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "FASTgram");
    SetWindowMinSize(360, 640);

    // Background color
    Color backgroundColor = GetColor(0xF1F5F9FF);

    // Fonts
    Font fontTitle = LoadFontEx("resources/fonts/LexendDeca.ttf", 50, NULL, 0);
    SetTextureFilter(fontTitle.texture, TEXTURE_FILTER_BILINEAR);
    Theme::appFont = fontTitle;

    float displaySize = 30.0f;
    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(fontTitle, "FASTgram", displaySize, spacing);
    Color headingColor = GetColor(0x0F172AFF);

    // Initialize ScreenManager and set the starting screen
    ScreenManager screenManager;
    LoginScreen* login = new LoginScreen(&screenManager, screenWidth, screenHeight);
    screenManager.setScreen(login);

    // FASTgram loop running at 60FPS
    SetTargetFPS(60);

    // Main loop (runs until the user presses ESC or closes the window)
    while (!WindowShouldClose()) {
        // --- 1. UPDATE LOGIC ---
        screenManager.update();

        // --- 2. DRAW GRAPHICS ---
        BeginDrawing();
        ClearBackground(backgroundColor);

        // Top app header
        Vector2 position = {
            (GetScreenWidth() - textSize.x) / 2.0f,
            20.0f
        };
        DrawTextEx(fontTitle, "FASTgram", position, displaySize, spacing, headingColor);

        ClearBackground(Theme::outerBackground());

        float appWidth = std::min(480.0f, (float)GetScreenWidth());
        float appX = (GetScreenWidth() - appWidth) / 2.0f;
        Rectangle appColumn = { appX, 0.0f, appWidth, (float)GetScreenHeight() };

        DrawRectangleRec(appColumn, Theme::background());
        if (GetScreenWidth() > 480) {
            DrawRectangleLinesEx(appColumn, 1.0f, Theme::border());
        }

        // Draw whichever screen is currently active
        screenManager.draw();

        EndDrawing();
    }

    UnloadFont(fontTitle);
    CloseWindow();

    return 0;
}