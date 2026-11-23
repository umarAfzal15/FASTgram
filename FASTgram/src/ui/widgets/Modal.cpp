#include "Modal.h"

Modal::Modal()
    : Widget({ 0.0f, 0.0f, 0.0f, 0.0f }),
    title(""),
    message(""),
    confirmText("Confirm"),
    cancelText("Cancel"),
    cardBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    confirmBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    cancelBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    visible(false),
    confirmed(false),
    cancelled(false)
{
}

void Modal::open(const std::string& modalTitle,
    const std::string& modalMessage,
    const std::string& confirmLabel,
    const std::string& cancelLabel) {
    title = modalTitle;
    message = modalMessage;
    confirmText = confirmLabel;
    cancelText = cancelLabel;

    visible = true;
    confirmed = false;
    cancelled = false;
}

void Modal::close() {
    visible = false;
}

bool Modal::isOpen() const {
    return visible;
}

bool Modal::isConfirmed() {
    if (confirmed) {
        confirmed = false;
        return true;
    }
    return false;
}

bool Modal::isCancelled() {
    if (cancelled) {
        cancelled = false;
        return true;
    }
    return false;
}

void Modal::update() {
    if (!visible) return;

    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    bounds = { 0.0f, 0.0f, screenW, screenH };

    // Center the dialog card in the middle of the window
    float cardW = 420.0f;
    float cardH = 200.0f;
    float cardX = (screenW - cardW) / 2.0f;
    float cardY = (screenH - cardH) / 2.0f;
    cardBounds = { cardX, cardY, cardW, cardH };

    // Place Cancel and Confirm buttons side-by-side at the bottom-right of the card
    float btnW = 110.0f;
    float btnH = 38.0f;
    float padding = 20.0f;
    float gap = 12.0f;
    float btnY = cardY + cardH - btnH - padding;

    confirmBtnBounds = { cardX + cardW - btnW - padding, btnY, btnW, btnH };
    cancelBtnBounds = { confirmBtnBounds.x - btnW - gap, btnY, btnW, btnH };

    Vector2 mouse = GetMousePosition();

    if (IsKeyPressed(KEY_ESCAPE)) {
        cancelled = true;
        close();
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, confirmBtnBounds)) {
            confirmed = true;
            close();
        }
        else if (CheckCollisionPointRec(mouse, cancelBtnBounds) || !CheckCollisionPointRec(mouse, cardBounds)) {
            cancelled = true;
            close();
        }
    }
}

void Modal::draw() const {
    if (!visible) return;

    DrawRectangleRec(bounds, Fade(BLACK, 0.55f));

    DrawRectangleRounded(cardBounds, 0.08f, 8, Theme::surface());
    DrawRectangleRoundedLinesEx(cardBounds, 0.08f, 8, 1.5f, Theme::border());

    Vector2 titlePos = { cardBounds.x + 24.0f, cardBounds.y + 22.0f };
    DrawTextEx(Theme::appFont, title.c_str(), titlePos, 22.0f, 1.0f, Theme::primaryText());

    Vector2 msgPos = { cardBounds.x + 24.0f, cardBounds.y + 62.0f };
    DrawTextEx(Theme::appFont, message.c_str(), msgPos, 17.0f, 1.0f, Theme::secondaryText());

    Vector2 mouse = GetMousePosition();

    Color cancelBg = CheckCollisionPointRec(mouse, cancelBtnBounds) ? Theme::surfaceHover() : Theme::background();
    DrawRectangleRounded(cancelBtnBounds, 0.2f, 8, cancelBg);
    DrawRectangleRoundedLinesEx(cancelBtnBounds, 0.2f, 8, 1.0f, Theme::border());

    // Center button label text inside each button rectangle
    Vector2 cancelSize = MeasureTextEx(Theme::appFont, cancelText.c_str(), 16.0f, 1.0f);
    Vector2 cancelTextPos = {
        cancelBtnBounds.x + (cancelBtnBounds.width - cancelSize.x) / 2.0f,
        cancelBtnBounds.y + (cancelBtnBounds.height - cancelSize.y) / 2.0f
    };
    DrawTextEx(Theme::appFont, cancelText.c_str(), cancelTextPos, 16.0f, 1.0f, Theme::primaryText());

    Color confirmBg = CheckCollisionPointRec(mouse, confirmBtnBounds) ? Theme::accentHover() : Theme::accent();
    Color confirmTextColor = Theme::isDark() ? Theme::background() : WHITE;
    DrawRectangleRounded(confirmBtnBounds, 0.2f, 8, confirmBg);

    Vector2 confirmSize = MeasureTextEx(Theme::appFont, confirmText.c_str(), 16.0f, 1.0f);
    Vector2 confirmTextPos = {
        confirmBtnBounds.x + (confirmBtnBounds.width - confirmSize.x) / 2.0f,
        confirmBtnBounds.y + (confirmBtnBounds.height - confirmSize.y) / 2.0f
    };
    DrawTextEx(Theme::appFont, confirmText.c_str(), confirmTextPos, 16.0f, 1.0f, confirmTextColor);
}