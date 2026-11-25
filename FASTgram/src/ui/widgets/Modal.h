#pragma once
#include "Widget.h"
#include "../src/core/Theme.h"
#include "raylib.h"
#include <string>

class Modal : public Widget {
private:
    std::string title;
    std::string message;
    std::string confirmText;
    std::string cancelText;

    Rectangle cardBounds;
    Rectangle confirmBtnBounds;
    Rectangle cancelBtnBounds;

    bool visible;
    bool confirmed;
    bool cancelled;

public:
    Modal();

    void open(const std::string& modalTitle,
        const std::string& modalMessage,
        const std::string& confirmLabel = "Confirm",
        const std::string& cancelLabel = "Cancel");
    void close();

    bool isOpen() const;
    bool isConfirmed();
    bool isCancelled();

    void update() override;
    void draw() const override;
};