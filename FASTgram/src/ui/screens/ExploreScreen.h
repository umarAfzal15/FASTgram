#pragma once
#include "Screen.h"
#include "../Widgets/ScrollView.h"
#include "../../services/PostManager.h"
#include "../../services/UserManager.h"
#include "../../models/SocialGraph.h"
#include "../../models/Account.h"
#include "raylib.h"
#include <vector>
#include <functional>

class ExploreScreen : public Screen {
private:
    PostManager* postManager;
    UserManager* userManager;
    SocialGraph* socialGraph;
    Account* currentUser;

    ScrollView scrollView;
    std::vector<Post*> visiblePosts;
    std::function<void(Post*)> onPostSelected;

    static constexpr float COLUMN_WIDTH = 480.0f;
    static constexpr float GAP = 4.0f;
    static constexpr int COLUMNS = 3;

    Rectangle getContentArea() const;
    void refreshPosts();
    void drawTile(Post* post, Rectangle tileRect, bool isHovered) const;

public:
    ExploreScreen(PostManager* posts = nullptr,
        UserManager* users = nullptr,
        SocialGraph* graph = nullptr,
        Account* viewer = nullptr,
        std::function<void(Post*)> onSelect = nullptr);

    void setCurrentUser(Account* viewer);
    void setOnPostSelected(std::function<void(Post*)> callback);

    void onEnter() override;
    void update() override;
    void draw() const override;
};