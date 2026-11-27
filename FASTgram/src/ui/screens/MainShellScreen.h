#pragma once
#include "Screen.h"
#include "PeopleListScreen.h"
#include "../src/ui/Widgets/Toast.h"
#include "../src/ui/Widgets/Modal.h"
#include "../src/core/Theme.h"
#include "../src/core/Logger.h"
#include "../../services/PostManager.h"
#include "../../models/SocialGraph.h"
#include "raylib.h"
#include <string>
#include <vector>

class ScreenManager;
class UserManager;
class FileManager;
class Account;

enum class ShellTab {
    Home = 0,
    Explore = 1,
    NewPost = 2,
    Notifications = 3,
    Profile = 4
};

class MainShellScreen : public Screen {
private:
    static const int MAX_DETAIL_SCREENS = 10;

    ScreenManager* screenManager;
    UserManager* userManager;
    FileManager* fileManager;
    Account* currentUser;

    PostManager ownedPostManager;
    SocialGraph ownedSocialGraph;

    PostManager* postManager;
    SocialGraph* socialGraph;

    ShellTab activeTab;
    std::string currentUsername;

    Rectangle frameBounds;
    Rectangle topBarBounds;
    Rectangle contentBounds;
    Rectangle bottomBarBounds;

    Rectangle chatBtnBounds;
    Rectangle campusBtnBounds;
    Rectangle tabBtnBounds[5];

    Screen* tabScreens[5];
    Screen* detailStack[MAX_DETAIL_SCREENS];
    int detailCount;
    std::vector<Screen*> pendingDeletes;

    Toast toast;
    Modal modal;

    void updateLayout();
    void drawTopBar() const;
    void drawBottomBar() const;
    void flushPendingDeletes();
    void initDefaultTabs();

public:
    MainShellScreen(ScreenManager* manager = nullptr, UserManager* users = nullptr, FileManager* files = nullptr, Account* user = nullptr);
    ~MainShellScreen();

    void setServices(PostManager* posts, SocialGraph* graph);
    void setCurrentUser(const std::string& username);
    void setTabScreen(ShellTab tab, Screen* screen);
    void setTabScreen(int tabIndex, Screen* screen);
    void setActiveTab(ShellTab tab);
    ShellTab getActiveTab() const;

    void pushDetailScreen(Screen* screen);
    void popDetailScreen();
    void clearDetailStack();

    void openPostDetail(int postId);
    void openProfile(int userId);
    void openPeopleList(int userId, PeopleListMode mode);

    Rectangle getContentBounds() const;
    void showToast(const std::string& msg, ToastType type = ToastType::Info);

    void update() override;
    void draw() const override;
};