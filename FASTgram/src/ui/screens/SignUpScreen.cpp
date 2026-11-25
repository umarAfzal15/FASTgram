#include "SignUpScreen.h"
#include "ScreenManager.h"
#include "LoginScreen.h"
#include "../src/core/Theme.h"
#include "../src/core/Logger.h"
#include <exception>

SignUpScreen::SignUpScreen(ScreenManager* manager, UserManager* users, FileManager* files, int screenWidth, int screenHeight)
    : screenManager(manager),
    userManager(users),
    fileManager(files),
    screenWidth(screenWidth),
    screenHeight(screenHeight),
    fullNameBox({ 0, 0, 300.0f, 45.0f }, "Full Name", false, 30, 18),
    usernameBox({ 0, 0, 300.0f, 45.0f }, "Username / Roll No", false, 20, 18),
    rollNoBox({ 0, 0, 300.0f, 45.0f }, "Roll No (e.g. i25-1234)", false, 10, 18),
    passwordBox({ 0, 0, 300.0f, 45.0f }, "Password (min 6 chars)", true, 20, 18),
    confirmPasswordBox({ 0, 0, 300.0f, 45.0f }, "Confirm Password", true, 20, 18),
    createAccountButton({ 0, 0, 300.0f, 45.0f }, "Create Account", ButtonStyle::Primary, 18),
    backToLoginButton({ 0, 0, 300.0f, 45.0f }, "Back to Login", ButtonStyle::Secondary, 16),
    isSuccess(false)
{
}

void SignUpScreen::update() {
    this->screenWidth = GetScreenWidth();
    this->screenHeight = GetScreenHeight();

    float centerX = this->screenWidth / 2.0f - 150.0f;
    float centerY = this->screenHeight / 2.0f;

    this->fullNameBox.setBounds({ centerX, centerY - 170.0f, 300.0f, 45.0f });
    this->usernameBox.setBounds({ centerX, centerY - 115.0f, 300.0f, 45.0f });
    this->rollNoBox.setBounds({ centerX, centerY - 60.0f, 300.0f, 45.0f });
    this->passwordBox.setBounds({ centerX, centerY - 5.0f, 300.0f, 45.0f });
    this->confirmPasswordBox.setBounds({ centerX, centerY + 50.0f, 300.0f, 45.0f });
    this->createAccountButton.setBounds({ centerX, centerY + 115.0f, 300.0f, 45.0f });
    this->backToLoginButton.setBounds({ centerX, centerY + 170.0f, 300.0f, 45.0f });

    this->fullNameBox.update();
    this->usernameBox.update();
    this->rollNoBox.update();
    this->passwordBox.update();
    this->confirmPasswordBox.update();
    this->createAccountButton.update();
    this->backToLoginButton.update();

    if (this->createAccountButton.isClicked()) {
        std::string fullName = this->fullNameBox.getText();
        std::string user = this->usernameBox.getText();
        std::string roll = this->rollNoBox.getText();
        std::string pass = this->passwordBox.getText();
        std::string confirmPass = this->confirmPasswordBox.getText();

        if (fullName.empty() || user.empty() || roll.empty() || pass.empty() || confirmPass.empty()) {
            this->isSuccess = false;
            this->statusMessage = "Please fill in all fields.";
        }
        else if (pass != confirmPass) {
            this->isSuccess = false;
            this->statusMessage = "Passwords do not match.";
        }
        else {
            try {
                bool registered = this->userManager->registerStudent(user, fullName, pass, roll);
                if (registered) {
                    this->fileManager->saveUsers(*this->userManager);
                    Logger::log("SignUp: " + user);
                    this->isSuccess = true;
                    this->statusMessage = "Account created! You can log in now.";
                }
                else {
                    this->isSuccess = false;
                    this->statusMessage = "Username or Roll Number already exists.";
                }
            }
            catch (const std::exception& ex) {
                this->isSuccess = false;
                this->statusMessage = ex.what();
            }
        }
    }

    if (this->backToLoginButton.isClicked()) {
        this->screenManager->setScreen(new LoginScreen(this->screenManager, this->userManager, this->fileManager, this->screenWidth, this->screenHeight));
        return;
    }
}

void SignUpScreen::draw() const {
    float spacing = 1.0f;

    float titleSize = 32.0f;
    Vector2 titleDimensions = MeasureTextEx(Theme::appFont, "Create Account", titleSize, spacing);
    Vector2 titlePos = {
        (this->screenWidth - titleDimensions.x) / 2.0f,
        this->screenHeight / 2.0f - 230.0f
    };
    DrawTextEx(Theme::appFont, "Create Account", titlePos, titleSize, spacing, Theme::accent());

    this->fullNameBox.draw();
    this->usernameBox.draw();
    this->rollNoBox.draw();
    this->passwordBox.draw();
    this->confirmPasswordBox.draw();
    this->createAccountButton.draw();
    this->backToLoginButton.draw();

    if (!this->statusMessage.empty()) {
        float msgSize = 16.0f;
        Vector2 msgDimensions = MeasureTextEx(Theme::appFont, this->statusMessage.c_str(), msgSize, spacing);
        Vector2 msgPos = {
            (this->screenWidth - msgDimensions.x) / 2.0f,
            this->screenHeight / 2.0f + 225.0f
        };
        Color msgColor = this->isSuccess ? Theme::success() : Theme::danger();
        DrawTextEx(Theme::appFont, this->statusMessage.c_str(), msgPos, msgSize, spacing, msgColor);
    }
}