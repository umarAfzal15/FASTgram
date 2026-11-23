#include "raylib.h"
#include "../src/core/Theme.h"
#include "../src/core/Logger.h"
#include "../services/UserManager.h"
#include "../services/FileManager.h"
#include "../src/ui/screens/ScreenManager.h"
#include "../src/ui/screens/LoginScreen.h"

int main() {
    Theme::loadConfig();

    // Setup resizable window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(Theme::getWindowWidth(), Theme::getWindowHeight(), "FASTgram");
    SetWindowMinSize(360, 640);
    SetExitKey(KEY_NULL); // So Esc goes back instead of closing the app
    SetTargetFPS(60);

    // Load app font
    Font fontTitle = LoadFontEx("resources/fonts/LexendDeca.ttf", 50, nullptr, 0);
    SetTextureFilter(fontTitle.texture, TEXTURE_FILTER_BILINEAR);
    Theme::appFont = fontTitle;

    // Start on login screen
    UserManager userManager;
    FileManager fileManager("data");
    fileManager.loadUsers(userManager);
    Logger::log("Application started");

    ScreenManager screenManager;
    screenManager.setScreen(new LoginScreen(&screenManager, &userManager, &fileManager, GetScreenWidth(), GetScreenHeight()));

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        screenManager.update();

        BeginDrawing();

        // Window background
        ClearBackground(Theme::outerBackground());

        // Keep app inside a centered 480px column
        float windowW = (float)GetScreenWidth();
        float windowH = (float)GetScreenHeight();
        float appWidth = (windowW < 480.0f) ? windowW : 480.0f;
        float appX = (windowW - appWidth) / 2.0f;

        Rectangle appColumn = { appX, 0.0f, appWidth, windowH };
        DrawRectangleRec(appColumn, Theme::background());

        if (windowW > 480.0f) {
            DrawRectangleLinesEx(appColumn, 1.0f, Theme::border());
        }

        screenManager.draw();

        EndDrawing();
    }

    UnloadFont(fontTitle);
    CloseWindow();

    return 0;
}