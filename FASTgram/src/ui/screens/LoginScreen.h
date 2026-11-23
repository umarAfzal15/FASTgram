#pragma once
#include "raylib.h"
#include <string>
#include "Screen.h"
#include "../src/core/Theme.h"
#include "../src/ui/widgets/Button.h"
#include "../src/ui/widgets/TextBox.h"

class ScreenManager;
class LoginScreen : public Screen {
private:
    ScreenManager* screenManager;
    int screenWidth;
    int screenHeight;
    TextBox usernameBox;
    TextBox passwordBox;
    Button loginButton;
    Button signUpButton;
    std::string statusMessage;

public:
    LoginScreen(ScreenManager* manager, int screenWidth, int screenHeight);

    void update() override;
    void draw() const override;
};