#pragma once
#include "Screen.h"
#include "../Widgets/Button.h"
#include "../Widgets/TextBox.h"
#include "../Widgets/Slider.h"
#include "../Widgets/Toast.h"
#include "../../services/PostManager.h"
#include "../../core/PostFactory.h"
#include "../../services/ImageStore.h"
#include "../../models/Account.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

class NewPostScreen : public Screen {
private:
    PostManager* postManager;
    Account* currentUser;
    std::string dataFolder;
    std::function<void()> onPostCreated;

    PostType selectedType;
    int selectedFilterIndex;
    std::string importedImagePath;

    Button photoModeBtn;
    Button textModeBtn;
    Button pollModeBtn;

    TextBox captionBox;

    Button filterNoneBtn;
    Button filterGrayBtn;
    Button filterBrightBtn;
    Button filterContrastBtn;
    Slider filterStrengthSlider;

    TextBox pollQuestionBox;
    TextBox pollOptionBoxes[4];
    int activePollOptionCount;
    Button addOptionBtn;
    Button removeOptionBtn;

    Button shareBtn;
    Toast toast;

    static constexpr float COLUMN_WIDTH = 480.0f;

    Rectangle getContentArea() const;
    int getCaptionLimit() const;
    void updateLayout();
    void switchMode(PostType newType);
    void resetForm();
    void handleShare();

public:
    NewPostScreen(PostManager* posts = nullptr,
        Account* user = nullptr,
        const std::string& dataDir = "data",
        std::function<void()> onCreated = nullptr);

    void setCurrentUser(Account* user);
    void setOnPostCreated(std::function<void()> callback);

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw() const override;
};