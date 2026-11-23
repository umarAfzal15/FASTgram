#include "SignUpScreen.h"
#include "ScreenManager.h"
#include "LoginScreen.h"
#include "../src/core/Theme.h"
#include <fstream>

SignUpScreen::SignUpScreen(ScreenManager* manager, int screenWidth, int screenHeight)
    : screenManager(manager),
    screenWidth(screenWidth),
    screenHeight(screenHeight),
    fullNameBox({ 0, 0, 300.0f, 45.0f }, "Full Name", false, 30, 18),
    usernameBox({ 0, 0, 300.0f, 45.0f }, "Username / Roll No", false, 20, 18),
    passwordBox({ 0, 0, 300.0f, 45.0f }, "Password", true, 20, 18),
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

    this->fullNameBox.setBounds({ centerX, centerY - 140.0f, 300.0f, 45.0f });
    this->usernameBox.setBounds({ centerX, centerY - 80.0f, 300.0f, 45.0f });
    this->passwordBox.setBounds({ centerX, centerY - 20.0f, 300.0f, 45.0f });
    this->confirmPasswordBox.setBounds({ centerX, centerY + 40.0f, 300.0f, 45.0f });
    this->createAccountButton.setBounds({ centerX, centerY + 110.0f, 300.0f, 45.0f });
    this->backToLoginButton.setBounds({ centerX, centerY + 170.0f, 300.0f, 45.0f });

    this->fullNameBox.update();
    this->usernameBox.update();
    this->passwordBox.update();
    this->confirmPasswordBox.update();
    this->createAccountButton.update();
    this->backToLoginButton.update();

    if (this->createAccountButton.isClicked()) {
        std::string fullName = this->fullNameBox.getText();
        std::string user = this->usernameBox.getText();
        std::string pass = this->passwordBox.getText();
        std::string confirmPass = this->confirmPasswordBox.getText();

        if (fullName.empty() || user.empty() || pass.empty() || confirmPass.empty()) {
            this->isSuccess = false;
            this->statusMessage = "Please fill in all fields.";
        }
        else if (pass != confirmPass) {
            this->isSuccess = false;
            this->statusMessage = "Passwords do not match.";
        }
        else {
            // Check for duplicate username in users.txt
            bool exists = false;
            std::ifstream inFile("data/users.txt");
            std::string savedUser, savedPass;
            while (inFile >> savedUser >> savedPass) {
                if (savedUser == user) {
                    exists = true;
                    break;
                }
            }
            inFile.close();

            if (exists) {
                this->isSuccess = false;
                this->statusMessage = "Username already exists.";
            }
            else {
                std::ofstream outFile("data/users.txt", std::ios::app);
                if (outFile.is_open()) {
                    outFile << user << " " << pass << "\n";
                    outFile.close();
                    this->isSuccess = true;
                    this->statusMessage = "Account created! You can log in now.";
                }
            }
        }
    }

    if (this->backToLoginButton.isClicked()) {
        this->screenManager->setScreen(new LoginScreen(this->screenManager, this->screenWidth, this->screenHeight));
        return;
    }
}

void SignUpScreen::draw() const {
    float spacing = 1.0f;

    float titleSize = 32.0f;
    Vector2 titleDimensions = MeasureTextEx(Theme::appFont, "Create Account", titleSize, spacing);
    Vector2 titlePos = {
        (this->screenWidth - titleDimensions.x) / 2.0f,
        this->screenHeight / 2.0f - 205.0f
    };
    DrawTextEx(Theme::appFont, "Create Account", titlePos, titleSize, spacing, Theme::accent());

    this->fullNameBox.draw();
    this->usernameBox.draw();
    this->passwordBox.draw();
    this->confirmPasswordBox.draw();
    this->createAccountButton.draw();
    this->backToLoginButton.draw();

    if (!this->statusMessage.empty()) {
        float msgSize = 16.0f;
        Vector2 msgDimensions = MeasureTextEx(Theme::appFont, this->statusMessage.c_str(), msgSize, spacing);
        Vector2 msgPos = {
            (this->screenWidth - msgDimensions.x) / 2.0f,
            this->screenHeight / 2.0f + 230.0f
        };
        Color msgColor = this->isSuccess ? Theme::success() : Theme::danger();
        DrawTextEx(Theme::appFont, this->statusMessage.c_str(), msgPos, msgSize, spacing, msgColor);
    }
}