#include "LoginScreen.h"
#include "ScreenManager.h"
#include "SignUpScreen.h"
#include "../src/core/Theme.h"
#include <exception>

LoginScreen::LoginScreen(ScreenManager* manager, UserManager* users, FileManager* files, int screenWidth, int screenHeight)
    : screenManager(manager),
      userManager(users),
      fileManager(files),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      usernameBox({ screenWidth / 2.0f - 150.0f, screenHeight / 2.0f - 80.0f, 300.0f, 45.0f }, "Username", false, 20, 18),
      passwordBox({ screenWidth / 2.0f - 150.0f, screenHeight / 2.0f - 20.0f, 300.0f, 45.0f }, "Password", true, 20, 18),
      loginButton({ screenWidth / 2.0f - 150.0f, screenHeight / 2.0f + 50.0f, 300.0f, 45.0f }, "Log In", ButtonStyle::Primary, 18),
      signUpButton({ screenWidth / 2.0f - 150.0f, screenHeight / 2.0f + 110.0f, 300.0f, 45.0f }, "Create New Account", ButtonStyle::Secondary, 16)
{
}

void LoginScreen::update() {
    this->screenWidth = GetScreenWidth();
    this->screenHeight = GetScreenHeight();

    float centerX = this->screenWidth / 2.0f - 150.0f;
    float centerY = this->screenHeight / 2.0f;

    this->usernameBox.setBounds({ centerX, centerY - 80.0f, 300.0f, 45.0f });
    this->passwordBox.setBounds({ centerX, centerY - 20.0f, 300.0f, 45.0f });
    this->loginButton.setBounds({ centerX, centerY + 50.0f, 300.0f, 45.0f });
    this->signUpButton.setBounds({ centerX, centerY + 110.0f, 300.0f, 45.0f });

    this->usernameBox.update();
    this->passwordBox.update();
    this->loginButton.update();
    this->signUpButton.update();

    if (this->loginButton.isClicked()) {
        std::string user = this->usernameBox.getText();
        std::string pass = this->passwordBox.getText();

        if (user.empty() || pass.empty()) {
            this->statusMessage = "Please enter both username and password.";
        }
        else {
            try {
                Account* loggedInUser = this->userManager->login(user, pass);
                this->statusMessage = "Welcome, " + loggedInUser->getDisplayName() + "!";
                // TODO: Save session and switch to MainShellScreen
            }
            catch (const std::exception& ex) {
                this->statusMessage = ex.what(); // Shows "Invalid username or password"
            }
        }
    }

    if (this->signUpButton.isClicked()) {
        this->screenManager->setScreen(new SignUpScreen(this->screenManager, this->userManager, this->fileManager, this->screenWidth, this->screenHeight));
        return;
    }
}

void LoginScreen::draw() const {
    float spacing = 1.0f;

    // 1. Centered "FASTgram" title using LexendDeca
    float titleSize = 36.0f;
    Vector2 titleDimensions = MeasureTextEx(Theme::appFont, "FASTgram", titleSize, spacing);
    Vector2 titlePos = {
        (this->screenWidth - titleDimensions.x) / 2.0f,
        this->screenHeight / 2.0f - 150.0f
    };
    DrawTextEx(Theme::appFont, "FASTgram", titlePos, titleSize, spacing, Theme::accent());

    // 2. Widgets
    this->usernameBox.draw();
    this->passwordBox.draw();
    this->loginButton.draw();
    this->signUpButton.draw();

    // 3. Status message below the button using LexendDeca
    if (!this->statusMessage.empty()) {
        float spacing = 1.0f;
        float msgSize = 16.0f;
        Vector2 msgDimensions = MeasureTextEx(Theme::appFont, this->statusMessage.c_str(), msgSize, spacing);
        Vector2 msgPos = {
            (this->screenWidth - msgDimensions.x) / 2.0f,
            this->screenHeight / 2.0f + 165.0f
        };
        DrawTextEx(Theme::appFont, this->statusMessage.c_str(), msgPos, msgSize, spacing, Theme::danger());
    }
}