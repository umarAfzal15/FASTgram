#pragma once
#include "Screen.h"
#include "../Widgets/ScrollView.h"
#include "../Widgets/TextBox.h"
#include "../Widgets/Button.h"
#include "../Widgets/Avatar.h"
#include "../../services/UserManager.h"
#include "../../models/SocialGraph.h"
#include "../../models/Account.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

enum class PeopleListMode {
    Followers = 0,
    Following = 1
};

class PeopleListScreen : public Screen {
private:
    int targetUserId;
    int viewerUserId;
    PeopleListMode mode;

    UserManager* userManager;
    SocialGraph* socialGraph;

    std::function<void()> onBack;
    std::function<void(int)> onUserSelected;

    Button backBtn;
    TextBox searchBox;
    ScrollView scrollView;

    std::vector<int> filteredUserIds;

    static constexpr float COLUMN_WIDTH = 480.0f;
    static constexpr float ROW_HEIGHT = 64.0f;
    static constexpr float ROW_GAP = 6.0f;

    Rectangle getContentArea() const;
    std::string makeInitials(const std::string& name) const;
    void updateLayout();
    void refreshList();

public:
    PeopleListScreen(int userId,
        PeopleListMode listMode,
        UserManager* users = nullptr,
        SocialGraph* graph = nullptr,
        int currentViewerId = -1,
        std::function<void()> onBackCallback = nullptr,
        std::function<void(int)> onSelectCallback = nullptr);

    void setViewerId(int currentViewerId);
    void setOnBack(std::function<void()> callback);
    void setOnUserSelected(std::function<void(int)> callback);

    void onEnter() override;
    void update() override;
    void draw() const override;
};