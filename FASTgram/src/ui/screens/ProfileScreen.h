#pragma once
#include "Screen.h"
#include "PeopleListScreen.h"
#include "../Widgets/Avatar.h"
#include "../Widgets/Button.h"
#include "../Widgets/ScrollView.h"
#include "../Widgets/Toast.h"
#include "../../services/UserManager.h"
#include "../../services/PostManager.h"
#include "../../models/TextFileManager.h"
#include "../../services/ImageStore.h"
#include "../../models/SocialGraph.h"
#include "../../models/Account.h"
#include "../../models/Post.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

class ProfileScreen : public Screen {
private:
    int profileUserId;
    int viewerUserId;

    UserManager* userManager;
    PostManager* postManager;
    SocialGraph* socialGraph;

    std::function<void()> onBack;
    std::function<void(int, PeopleListMode)> onOpenPeopleList;
    std::function<void(int)> onPostSelected;
    std::function<void()> onEditProfile;

    Button backBtn;
    Button followBtn;
    Button editBtn;
    Button exportBtn;
    ScrollView scrollView;
    Toast toast;

    Rectangle postsStatRect;
    Rectangle followersStatRect;
    Rectangle followingStatRect;

    std::vector<Post*> userPosts;

    static constexpr float COLUMN_WIDTH = 480.0f;
    static constexpr int COLUMNS = 3;
    static constexpr float GAP = 3.0f;

    Rectangle getContentArea() const;
    std::string makeInitials(const std::string& name) const;
    bool canViewFullProfile(const Account* target) const;
    void refreshPosts();
    void updateLayout(bool isOwnProfile);
    void drawTile(Post* post, Rectangle tileRect, bool isHovered) const;

public:
    ProfileScreen(int userId,
        UserManager* users = nullptr,
        PostManager* posts = nullptr,
        SocialGraph* graph = nullptr,
        int currentViewerId = -1,
        std::function<void()> onBackCallback = nullptr,
        std::function<void(int, PeopleListMode)> onPeopleCallback = nullptr,
        std::function<void(int)> onPostCallback = nullptr,
        std::function<void()> onEditCallback = nullptr);

    void setViewerId(int currentViewerId);
    void setOnBack(std::function<void()> callback);
    void setOnOpenPeopleList(std::function<void(int, PeopleListMode)> callback);
    void setOnPostSelected(std::function<void(int)> callback);
    void setOnEditProfile(std::function<void()> callback);

    void onEnter() override;
    void update() override;
    void draw() const override;
};