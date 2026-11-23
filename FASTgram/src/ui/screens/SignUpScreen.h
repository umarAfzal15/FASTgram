#pragma once
#include <string>
#include "Screen.h"
#include "../widgets/TextBox.h"
#include "../widgets/Button.h"
#include "../../services/UserManager.h"
#include "../../services/FileManager.h"

class ScreenManager; // Forward declaration to avoid circular include

class SignUpScreen : public Screen {
private:
    ScreenManager* screenManager;
    UserManager* userManager;
    FileManager* fileManager;
    int screenWidth;
    int screenHeight;

    TextBox fullNameBox;
    TextBox usernameBox;
    TextBox rollNoBox;
    TextBox passwordBox;
    TextBox confirmPasswordBox;

    Button createAccountButton;
    Button backToLoginButton;

    std::string statusMessage;
    bool isSuccess;

public:
    SignUpScreen(ScreenManager* manager, UserManager* users, FileManager* files, int screenWidth, int screenHeight);

    void update() override;
    void draw() const override;
};