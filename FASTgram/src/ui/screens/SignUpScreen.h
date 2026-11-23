#pragma once
#include <string>
#include "Screen.h"
#include "../src/ui/widgets/TextBox.h"
#include "../src/ui/widgets/Button.h"

class ScreenManager; // Forward declaration to avoid circular include

class SignUpScreen : public Screen {
private:
    ScreenManager* screenManager;
    int screenWidth;
    int screenHeight;

    TextBox fullNameBox;
    TextBox usernameBox;
    TextBox passwordBox;
    TextBox confirmPasswordBox;

    Button createAccountButton;
    Button backToLoginButton;

    std::string statusMessage;
    bool isSuccess;

public:
    SignUpScreen(ScreenManager* manager, int screenWidth, int screenHeight);

    void update() override;
    void draw() const override;
};