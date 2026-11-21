#include "raylib.h"
#include <string>
using namespace std;

int main() {
    // Window dimensions
    const int screenWidth = 480;
    const int screenHeight = 820;

    // Initialize the window and bring it to the screen
    InitWindow(screenWidth, screenHeight, "FASTgram");
    SetWindowMinSize(360, 640);

    //background color

    Color backgroundColor = GetColor(0xF1F5F9FF);


    //FONTS sizes
    Font fontTitle = LoadFontEx("resources/fonts/Roboto.ttf", 50, NULL, 0);

    SetTextureFilter(fontTitle.texture, TEXTURE_FILTER_BILINEAR);

    //Vector2 position = { 190, 2 };
    float displaySize = 30.0;
    float spacing = 1.0;
    Vector2 textSize = MeasureTextEx(fontTitle, "FASTagram", displaySize, spacing);
    Vector2 position = {
        (GetScreenWidth() - textSize.x) / 2.0f,
        2.0f
    };
    Color headingColor = GetColor(0x0F172AFF);

    // game/FASTgram loop running at 60FPS
    SetTargetFPS(60);

    // Main game loop (runs until the user presses ESC or closes the window)
    while (!WindowShouldClose()) {
        // --- 1. UPDATE LOGIC ---
        // Update variables, positions, and inputs here


        // --- 2. DRAW GRAPHICS ---
        BeginDrawing();
        ClearBackground(backgroundColor);

        //DrawText("FASTgram!", 200, 400, 24, DARKGRAY);
        DrawTextEx(fontTitle, "FASTgram", position, displaySize, spacing, headingColor);
        EndDrawing();
    }


    UnloadFont(fontTitle);
    // closing the window
    CloseWindow();

    return 0;
}