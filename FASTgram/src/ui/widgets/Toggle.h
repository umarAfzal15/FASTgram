#pragma once
#include "Widget.h"

class Toggle : public Widget {
private:
    bool toggled;

public:
    Toggle(Rectangle bounds = { 0, 0, 50, 28 }, bool initialState = false);

    void update() override;
    void draw() const override;

    bool isToggled() const;
    void setToggled(bool val);
};