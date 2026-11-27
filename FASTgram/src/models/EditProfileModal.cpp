#include "EditProfileModal.h"
#include "../src/core/Theme.h"

EditProfileModal::EditProfileModal()
    : Widget({ 0, 0, 0, 0 }),
    targetAccount(nullptr),
    visible(false),
    saved(false),
    cancelled(false),
    pendingPrivate(false),
    errorMessage(""),
    cardBounds({ 0, 0, 460, 380 }),
    displayNameBox({ 0, 0, 400, 42 }, "Display Name", false, 40, 18),
    bioBox({ 0, 0, 400, 42 }, "Bio (max 150 chars)", false, 150, 16),
    privacyToggleBtn({ 0, 0, 400, 40 }, "Privacy: Public", ButtonStyle::Secondary, 16),
    saveBtn({ 0, 0, 190, 42 }, "Save Changes", ButtonStyle::Primary, 17),
    cancelBtn({ 0, 0, 190, 42 }, "Cancel", ButtonStyle::Secondary, 17)
{
    updateLayout();
}

void EditProfileModal::updateLayout()
{
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    if (screenW <= 0.0f) screenW = 1280.0f;
    if (screenH <= 0.0f) screenH = 800.0f;

    bounds = { 0, 0, screenW, screenH };

    float cardW = 460.0f;
    float cardH = 390.0f;
    float cardX = (screenW - cardW) * 0.5f;
    float cardY = (screenH - cardH) * 0.5f;
    cardBounds = { cardX, cardY, cardW, cardH };

    float innerX = cardX + 30.0f;
    float innerW = cardW - 60.0f;

    displayNameBox.setBounds({ innerX, cardY + 88.0f, innerW, 42.0f });
    bioBox.setBounds({ innerX, cardY + 162.0f, innerW, 42.0f });
    privacyToggleBtn.setBounds({ innerX, cardY + 224.0f, innerW, 40.0f });

    float btnW = (innerW - 16.0f) * 0.5f;
    float btnY = cardY + cardH - 64.0f;
    cancelBtn.setBounds({ innerX, btnY, btnW, 42.0f });
    saveBtn.setBounds({ innerX + btnW + 16.0f, btnY, btnW, 42.0f });
}

void EditProfileModal::open(Account* account)
{
    targetAccount = account;
    saved = false;
    cancelled = false;
    errorMessage = "";

    if (targetAccount != nullptr)
    {
        displayNameBox.setText(targetAccount->getDisplayName());
        bioBox.setText(targetAccount->getBio());
        pendingPrivate = targetAccount->getIsPrivate();
        privacyToggleBtn.setText(pendingPrivate ? "Account Privacy: Private" : "Account Privacy: Public");
        displayNameBox.setFocused(true);
        bioBox.setFocused(false);
        visible = true;
        updateLayout();
    }
    else
    {
        visible = false;
    }
}

void EditProfileModal::close()
{
    visible = false;
    targetAccount = nullptr;
    displayNameBox.setFocused(false);
    bioBox.setFocused(false);
}

bool EditProfileModal::isOpen() const
{
    return visible;
}

bool EditProfileModal::isSaved()
{
    if (saved)
    {
        saved = false;
        return true;
    }
    return false;
}

bool EditProfileModal::isCancelled()
{
    if (cancelled)
    {
        cancelled = false;
        return true;
    }
    return false;
}

void EditProfileModal::update()
{
    if (!visible || targetAccount == nullptr)
    {
        return;
    }

    updateLayout();

    if (IsKeyPressed(KEY_ESCAPE))
    {
        cancelled = true;
        close();
        return;
    }

    displayNameBox.update();
    bioBox.update();
    privacyToggleBtn.update();
    saveBtn.update();
    cancelBtn.update();

    if (privacyToggleBtn.isClicked())
    {
        pendingPrivate = !pendingPrivate;
        privacyToggleBtn.setText(pendingPrivate ? "Account Privacy: Private" : "Account Privacy: Public");
    }

    if (cancelBtn.isClicked())
    {
        cancelled = true;
        close();
        return;
    }

    if (saveBtn.isClicked() || IsKeyPressed(KEY_ENTER))
    {
        std::string newName = displayNameBox.getText();
        if (newName.empty())
        {
            errorMessage = "Display name cannot be empty.";
            return;
        }

        targetAccount->setDisplayName(newName);
        targetAccount->setBio(bioBox.getText());
        targetAccount->setPrivate(pendingPrivate);

        saved = true;
        close();
    }
}

void EditProfileModal::draw() const
{
    if (!visible || targetAccount == nullptr)
    {
        return;
    }

    // Dimmed modal backdrop
    DrawRectangleRec(bounds, { 0, 0, 0, 150 });

    // Modal card
    DrawRectangleRounded(cardBounds, 0.06f, 8, WHITE);
    DrawRectangleRoundedLinesEx(cardBounds, 0.06f, 8, 1.5f, { 220, 220, 225, 255 });

    float innerX = cardBounds.x + 30.0f;

    // Header
    DrawText("Edit Profile", static_cast<int>(innerX), static_cast<int>(cardBounds.y + 24.0f), 22, { 24, 24, 28, 255 });
    DrawLine(static_cast<int>(cardBounds.x), static_cast<int>(cardBounds.y + 56.0f),
        static_cast<int>(cardBounds.x + cardBounds.width), static_cast<int>(cardBounds.y + 56.0f),
        { 235, 235, 240, 255 });

    // Labels & inputs
    DrawText("Display Name", static_cast<int>(innerX), static_cast<int>(cardBounds.y + 68.0f), 14, { 95, 95, 105, 255 });
    displayNameBox.draw();

    DrawText("Bio", static_cast<int>(innerX), static_cast<int>(cardBounds.y + 142.0f), 14, { 95, 95, 105, 255 });
    bioBox.draw();

    privacyToggleBtn.draw();

    if (!errorMessage.empty())
    {
        DrawText(errorMessage.c_str(), static_cast<int>(innerX), static_cast<int>(cardBounds.y + 276.0f), 14, { 220, 53, 69, 255 });
    }

    cancelBtn.draw();
    saveBtn.draw();
}