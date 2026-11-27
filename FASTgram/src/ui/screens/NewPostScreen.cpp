#include "NewPostScreen.h"
#include "../../core/Theme.h"
#include <stdexcept>
#include <algorithm>

NewPostScreen::NewPostScreen(PostManager* posts,
    Account* user,
    const std::string& dataDir,
    std::function<void()> onCreated)
    : postManager(posts),
    currentUser(user),
    dataFolder(dataDir),
    onPostCreated(onCreated),
    selectedType(PostType::Photo),
    selectedFilterIndex(0),
    importedImagePath(""),
    photoModeBtn({ 0, 0, 140, 36 }, "Photo", ButtonStyle::Primary, 16),
    textModeBtn({ 0, 0, 140, 36 }, "Text", ButtonStyle::Secondary, 16),
    pollModeBtn({ 0, 0, 140, 36 }, "Poll", ButtonStyle::Secondary, 16),
    captionBox({ 0, 0, 440, 74 }, "Write a caption...", false, 200, 16),
    filterNoneBtn({ 0, 0, 102, 30 }, "Normal", ButtonStyle::Primary, 14),
    filterGrayBtn({ 0, 0, 102, 30 }, "B&W", ButtonStyle::Secondary, 14),
    filterBrightBtn({ 0, 0, 102, 30 }, "Bright", ButtonStyle::Secondary, 14),
    filterContrastBtn({ 0, 0, 102, 30 }, "Contrast", ButtonStyle::Secondary, 14),
    filterStrengthSlider({ 0, 0, 440, 22 }, 0.5f),
    pollQuestionBox({ 0, 0, 440, 40 }, "Ask a poll question...", false, 140, 16),
    pollOptionBoxes{
        TextBox({ 0, 0, 440, 38 }, "Option 1", false, 60, 15),
        TextBox({ 0, 0, 440, 38 }, "Option 2", false, 60, 15),
        TextBox({ 0, 0, 440, 38 }, "Option 3", false, 60, 15),
        TextBox({ 0, 0, 440, 38 }, "Option 4", false, 60, 15)
    },
    activePollOptionCount(2),
    addOptionBtn({ 0, 0, 130, 32 }, "+ Add Option", ButtonStyle::Secondary, 14),
    removeOptionBtn({ 0, 0, 130, 32 }, "- Remove", ButtonStyle::Secondary, 14),
    shareBtn({ 0, 0, 440, 44 }, "Share Post", ButtonStyle::Primary, 18) {
    updateLayout();
}

void NewPostScreen::setCurrentUser(Account* user) {
    currentUser = user;
}

void NewPostScreen::setOnPostCreated(std::function<void()> callback) {
    onPostCreated = callback;
}

Rectangle NewPostScreen::getContentArea() const {
    float screenW = (float)Theme::getWindowWidth();
    float screenH = (float)Theme::getWindowHeight();
    float colX = (screenW - COLUMN_WIDTH) / 2.0f;
    if (colX < 0.0f) colX = 0.0f;

    float topBarH = 56.0f;
    float bottomBarH = 56.0f;
    float contentH = screenH - topBarH - bottomBarH;
    if (contentH < 100.0f) contentH = 100.0f;

    return { colX, topBarH, COLUMN_WIDTH, contentH };
}

int NewPostScreen::getCaptionLimit() const {
    if (selectedType == PostType::Text) return 500;
    if (selectedType == PostType::Poll) return 200;
    return 200;
}

void NewPostScreen::switchMode(PostType newType) {
    selectedType = newType;
    photoModeBtn.setStyle(selectedType == PostType::Photo ? ButtonStyle::Primary : ButtonStyle::Secondary);
    textModeBtn.setStyle(selectedType == PostType::Text ? ButtonStyle::Primary : ButtonStyle::Secondary);
    pollModeBtn.setStyle(selectedType == PostType::Poll ? ButtonStyle::Primary : ButtonStyle::Secondary);

    std::string currentText = captionBox.getText();
    int limit = getCaptionLimit();
    if ((int)currentText.size() > limit) {
        currentText = currentText.substr(0, limit);
    }
    captionBox = TextBox(captionBox.getBounds(),
        selectedType == PostType::Text ? "Write your text post..." : "Write a caption...",
        false,
        limit,
        16);
    captionBox.setText(currentText);
}

void NewPostScreen::resetForm() {
    captionBox.clear();
    pollQuestionBox.clear();
    for (int i = 0; i < 4; i++) {
        pollOptionBoxes[i].clear();
    }
    activePollOptionCount = 2;
    selectedFilterIndex = 0;
    filterStrengthSlider.setValue(0.5f);
    importedImagePath.clear();
    switchMode(PostType::Photo);
}

void NewPostScreen::updateLayout() {
    Rectangle area = getContentArea();
    float pad = 20.0f;
    float innerW = area.width - pad * 2.0f;
    float x = area.x + pad;
    float y = area.y + 14.0f;

    float modeGap = 10.0f;
    float modeW = (innerW - modeGap * 2.0f) / 3.0f;
    photoModeBtn.setBounds({ x, y, modeW, 36.0f });
    textModeBtn.setBounds({ x + modeW + modeGap, y, modeW, 36.0f });
    pollModeBtn.setBounds({ x + (modeW + modeGap) * 2.0f, y, modeW, 36.0f });
    y += 48.0f;

    if (selectedType == PostType::Photo) {
        y += 170.0f;

        float fGap = 8.0f;
        float fW = (innerW - fGap * 3.0f) / 4.0f;
        filterNoneBtn.setBounds({ x, y, fW, 30.0f });
        filterGrayBtn.setBounds({ x + fW + fGap, y, fW, 30.0f });
        filterBrightBtn.setBounds({ x + (fW + fGap) * 2.0f, y, fW, 30.0f });
        filterContrastBtn.setBounds({ x + (fW + fGap) * 3.0f, y, fW, 30.0f });
        y += 42.0f;

        filterStrengthSlider.setBounds({ x, y + 16.0f, innerW, 22.0f });
        y += 48.0f;

        captionBox.setBounds({ x, y, innerW, 74.0f });
        y += 96.0f;
    }
    else if (selectedType == PostType::Text) {
        captionBox.setBounds({ x, y, innerW, 180.0f });
        y += 204.0f;
    }
    else if (selectedType == PostType::Poll) {
        captionBox.setBounds({ x, y, innerW, 60.0f });
        y += 82.0f;

        pollQuestionBox.setBounds({ x, y, innerW, 38.0f });
        y += 48.0f;

        for (int i = 0; i < activePollOptionCount; i++) {
            pollOptionBoxes[i].setBounds({ x, y, innerW, 36.0f });
            y += 42.0f;
        }

        addOptionBtn.setBounds({ x, y, 130.0f, 30.0f });
        removeOptionBtn.setBounds({ x + 140.0f, y, 130.0f, 30.0f });
        y += 42.0f;
    }

    float shareY = std::max(y + 6.0f, area.y + area.height - 60.0f);
    shareBtn.setBounds({ x, shareY, innerW, 44.0f });
}

void NewPostScreen::handleShare() {
    if (!postManager) {
        toast.show("Post manager unavailable", ToastType::Error);
        return;
    }

    try {
        PostData data;
        data.authorId = currentUser ? currentUser->getId() : 1;
        data.caption = captionBox.getText();

        if (selectedType == PostType::Photo) {
            if (importedImagePath.empty()) {
                throw std::runtime_error("Drop an image file into the preview box first");
            }
            data.imagePath = importedImagePath;
        }
        else if (selectedType == PostType::Poll) {
            data.question = pollQuestionBox.getText();
            for (int i = 0; i < activePollOptionCount; i++) {
                std::string opt = pollOptionBoxes[i].getText();
                if (!opt.empty()) {
                    data.options.push_back(opt);
                }
            }
        }

        Post* created = PostFactory::create(selectedType, data);
        postManager->addPost(created);
        postManager->save(dataFolder);

        toast.show("Post shared!", ToastType::Success);
        resetForm();

        if (onPostCreated) {
            onPostCreated();
        }
    }
    catch (const std::exception& e) {
        toast.show(e.what(), ToastType::Error);
    }
}

void NewPostScreen::onEnter() {
    updateLayout();
}

void NewPostScreen::onExit() {
    captionBox.setFocused(false);
    pollQuestionBox.setFocused(false);
    for (int i = 0; i < 4; i++) {
        pollOptionBoxes[i].setFocused(false);
    }
}

void NewPostScreen::update() {
    updateLayout();

    photoModeBtn.update();
    textModeBtn.update();
    pollModeBtn.update();

    if (photoModeBtn.isClicked()) switchMode(PostType::Photo);
    if (textModeBtn.isClicked()) switchMode(PostType::Text);
    if (pollModeBtn.isClicked()) switchMode(PostType::Poll);

    if (selectedType == PostType::Photo) {
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0) {
                std::string savedPath = ImageStore::importImage(droppedFiles.paths[0], "posts");
                if (!savedPath.empty()) {
                    importedImagePath = savedPath;
                }
                else {
                    toast.show("Failed to import image", ToastType::Error);
                }
            }
            UnloadDroppedFiles(droppedFiles);
        }

        filterNoneBtn.update();
        filterGrayBtn.update();
        filterBrightBtn.update();
        filterContrastBtn.update();
        filterStrengthSlider.update();

        if (filterNoneBtn.isClicked()) selectedFilterIndex = 0;
        if (filterGrayBtn.isClicked()) selectedFilterIndex = 1;
        if (filterBrightBtn.isClicked()) selectedFilterIndex = 2;
        if (filterContrastBtn.isClicked()) selectedFilterIndex = 3;

        filterNoneBtn.setStyle(selectedFilterIndex == 0 ? ButtonStyle::Primary : ButtonStyle::Secondary);
        filterGrayBtn.setStyle(selectedFilterIndex == 1 ? ButtonStyle::Primary : ButtonStyle::Secondary);
        filterBrightBtn.setStyle(selectedFilterIndex == 2 ? ButtonStyle::Primary : ButtonStyle::Secondary);
        filterContrastBtn.setStyle(selectedFilterIndex == 3 ? ButtonStyle::Primary : ButtonStyle::Secondary);
    }
    else if (selectedType == PostType::Poll) {
        pollQuestionBox.update();
        for (int i = 0; i < activePollOptionCount; i++) {
            pollOptionBoxes[i].update();
        }

        addOptionBtn.setEnabled(activePollOptionCount < 4);
        removeOptionBtn.setEnabled(activePollOptionCount > 2);
        addOptionBtn.update();
        removeOptionBtn.update();

        if (addOptionBtn.isClicked() && activePollOptionCount < 4) {
            activePollOptionCount++;
        }
        if (removeOptionBtn.isClicked() && activePollOptionCount > 2) {
            pollOptionBoxes[activePollOptionCount - 1].clear();
            activePollOptionCount--;
        }
    }

    captionBox.update();

    shareBtn.update();
    if (shareBtn.isClicked()) {
        handleShare();
    }

    toast.update();
}

void NewPostScreen::draw() const {
    Rectangle area = getContentArea();
    DrawRectangleRec(area, Theme::background());

    photoModeBtn.draw();
    textModeBtn.draw();
    pollModeBtn.draw();

    float pad = 20.0f;
    float innerW = area.width - pad * 2.0f;
    float x = area.x + pad;

    if (selectedType == PostType::Photo) {
        Rectangle previewBox = { x, area.y + 62.0f, innerW, 160.0f };
        DrawRectangleRounded(previewBox, 0.08f, 8, Theme::surface());
        DrawRectangleRoundedLinesEx(previewBox, 0.08f, 8, 1.5f, Theme::border());

        if (!importedImagePath.empty()) {
            Texture2D tex = ImageStore::getTexture(importedImagePath);
            float scale = std::min((previewBox.width - 12.0f) / (float)tex.width,
                (previewBox.height - 12.0f) / (float)tex.height);
            float drawW = tex.width * scale;
            float drawH = tex.height * scale;
            Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dst = {
                previewBox.x + (previewBox.width - drawW) / 2.0f,
                previewBox.y + (previewBox.height - drawH) / 2.0f,
                drawW,
                drawH
            };

            Color tint = WHITE;
            float strength = filterStrengthSlider.getValue();
            if (selectedFilterIndex == 1) {
                unsigned char g = (unsigned char)(255 - strength * 90.0f);
                tint = { g, g, g, 255 };
            }
            else if (selectedFilterIndex == 2) {
                unsigned char b = (unsigned char)std::min(255.0f, 200.0f + strength * 55.0f);
                tint = { b, b, b, 255 };
            }
            else if (selectedFilterIndex == 3) {
                tint = { 245, 235, 255, 255 };
            }

            DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tint);
        }
        else {
            const char* dropPrompt = "Drag & drop an image file here";
            Vector2 promptSize = MeasureTextEx(Theme::appFont, dropPrompt, 15.0f, 1.0f);
            DrawTextEx(Theme::appFont,
                dropPrompt,
                { previewBox.x + (previewBox.width - promptSize.x) / 2.0f,
                  previewBox.y + (previewBox.height - promptSize.y) / 2.0f },
                15.0f,
                1.0f,
                Theme::secondaryText());
        }

        filterNoneBtn.draw();
        filterGrayBtn.draw();
        filterBrightBtn.draw();
        filterContrastBtn.draw();

        Rectangle sBounds = filterStrengthSlider.getBounds();
        int pct = (int)(filterStrengthSlider.getValue() * 100.0f + 0.5f);
        std::string strengthLabel = "Filter Strength: " + std::to_string(pct) + "%";
        DrawTextEx(Theme::appFont, strengthLabel.c_str(), { sBounds.x, sBounds.y - 18.0f }, 13.0f, 1.0f, Theme::secondaryText());
        filterStrengthSlider.draw();
    }
    else if (selectedType == PostType::Poll) {
        pollQuestionBox.draw();
        for (int i = 0; i < activePollOptionCount; i++) {
            pollOptionBoxes[i].draw();
        }
        addOptionBtn.draw();
        removeOptionBtn.draw();
    }

    captionBox.draw();

    Rectangle capBounds = captionBox.getBounds();
    int currentLen = (int)captionBox.getText().size();
    int maxLen = getCaptionLimit();
    std::string counterText = std::to_string(currentLen) + "/" + std::to_string(maxLen);
    Vector2 counterSize = MeasureTextEx(Theme::appFont, counterText.c_str(), 13.0f, 1.0f);
    Color counterColor = (currentLen >= maxLen) ? Theme::danger() : Theme::secondaryText();
    DrawTextEx(Theme::appFont,
        counterText.c_str(),
        { capBounds.x + capBounds.width - counterSize.x, capBounds.y + capBounds.height + 4.0f },
        13.0f,
        1.0f,
        counterColor);

    shareBtn.draw();
    toast.draw();
}