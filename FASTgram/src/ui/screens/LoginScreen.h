#pragma once
#include "raylib.h"
#include <string>
#include "Screen.h"
#include "../../core/Theme.h"
#include "../widgets/Button.h"
#include "../widgets/TextBox.h"
#include "../../services/UserManager.h"
#include "../../services/FileManager.h"

class ScreenManager;

class LoginScreen : public Screen {
private:
    ScreenManager* screenManager;
    UserManager* userManager;
    FileManager* fileManager;
    int screenWidth;
    int screenHeight;
    TextBox usernameBox;
    TextBox passwordBox;
    Button loginButton;
    Button signUpButton;
    std::string statusMessage;

public:
    LoginScreen(ScreenManager* manager, UserManager* users, FileManager* files, int screenWidth, int screenHeight);

    void update() override;
    void draw() const override;
};