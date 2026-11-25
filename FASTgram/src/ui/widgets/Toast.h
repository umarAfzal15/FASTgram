#pragma once
#include "Widget.h"
#include "raylib.h"
#include <string>

enum class ToastType {
    Info,
    Success,
    Error
};

class Toast : public Widget {
private:
    std::string message;
    ToastType type;
    float duration;
    float timer;
    bool visible;

public:
    Toast();

    void show(const std::string& msg, ToastType toastType = ToastType::Info, float seconds = 3.0f);
    void hide();
    bool isVisible() const;

    void update() override;
    void draw() const override;
};