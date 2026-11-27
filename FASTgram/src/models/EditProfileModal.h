#pragma once
#include "ui/widgets/Widget.h"
#include "ui/widgets/TextBox.h"
#include "ui/widgets/Button.h"
#include "models/Account.h"
#include "raylib.h"
#include <string>

class EditProfileModal : public Widget {
private:
    Account* targetAccount;
    bool visible;
    bool saved;
    bool cancelled;
    bool pendingPrivate;
    std::string errorMessage;

    Rectangle cardBounds;
    TextBox displayNameBox;
    TextBox bioBox;
    Button privacyToggleBtn;
    Button saveBtn;
    Button cancelBtn;

    void updateLayout();

public:
    EditProfileModal();

    void open(Account* account);
    void close();

    bool isOpen() const;
    bool isSaved();
    bool isCancelled();

    void update() override;
    void draw() const override;
};