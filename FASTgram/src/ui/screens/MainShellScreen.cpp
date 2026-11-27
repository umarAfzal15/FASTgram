#include "MainShellScreen.h"
#include "ScreenManager.h"
#include "LoginScreen.h"
#include "SettingsScreen.h"
#include "HomeScreen.h"
#include "ExploreScreen.h"
#include "ProfileScreen.h"
#include "PostDetailScreen.h"
#include "../../models/Account.h"

MainShellScreen::MainShellScreen(ScreenManager* manager, UserManager* users, FileManager* files, Account* user)
    : screenManager(manager),
    userManager(users),
    fileManager(files),
    currentUser(user),
    postManager(&ownedPostManager),
    socialGraph(&ownedSocialGraph),
    activeTab(ShellTab::Home),
    currentUsername(user != nullptr ? user->getUsername() : ""),
    frameBounds({ 0.0f, 0.0f, 480.0f, 0.0f }),
    topBarBounds({ 0.0f, 0.0f, 480.0f, 56.0f }),
    contentBounds({ 0.0f, 56.0f, 480.0f, 0.0f }),
    bottomBarBounds({ 0.0f, 0.0f, 480.0f, 60.0f }),
    chatBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    campusBtnBounds({ 0.0f, 0.0f, 0.0f, 0.0f }),
    detailCount(0)
{
    for (int i = 0; i < 5; i++) {
        tabBtnBounds[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
        tabScreens[i] = nullptr;
    }

    for (int i = 0; i < MAX_DETAIL_SCREENS; i++) {
        detailStack[i] = nullptr;
    }

    updateLayout();
    ownedPostManager.load("data");
    initDefaultTabs();
}

MainShellScreen::~MainShellScreen() {
    ownedPostManager.purgeExpired();
    ownedPostManager.save("data");
    clearDetailStack();
    flushPendingDeletes();
    for (int i = 0; i < 5; i++) {
        delete tabScreens[i];
        tabScreens[i] = nullptr;
    }
}

void MainShellScreen::flushPendingDeletes() {
    for (Screen* s : pendingDeletes) {
        delete s;
    }
    pendingDeletes.clear();
}

void MainShellScreen::initDefaultTabs() {
    if (currentUser == nullptr) return;
    int viewerId = currentUser->getId();

    HomeScreen* home = new HomeScreen(
        this,
        postManager,
        socialGraph,
        nullptr,
        currentUser,
        userManager
    );
    setTabScreen(ShellTab::Home, home);

    ExploreScreen* explore = new ExploreScreen(
        postManager,
        userManager,
        socialGraph,
        currentUser,
        [this](Post* selected) {
            if (selected != nullptr) {
                openPostDetail(selected->getId());
            }
        }
    );
    setTabScreen(ShellTab::Explore, explore);

    ProfileScreen* ownProfile = new ProfileScreen(
        viewerId,
        userManager,
        postManager,
        socialGraph,
        viewerId,
        [this]() { setActiveTab(ShellTab::Home); },
        [this](int uid, PeopleListMode mode) { openPeopleList(uid, mode); },
        [this](int pid) { openPostDetail(pid); },
        [this]() { pushDetailScreen(new SettingsScreen(contentBounds, currentUsername)); }
    );
    setTabScreen(ShellTab::Profile, ownProfile);
}

void MainShellScreen::setServices(PostManager* posts, SocialGraph* graph) {
    if (posts != nullptr) postManager = posts;
    if (graph != nullptr) socialGraph = graph;
    initDefaultTabs();
}

void MainShellScreen::setCurrentUser(const std::string& username) {
    currentUsername = username;
}

void MainShellScreen::setTabScreen(ShellTab tab, Screen* screen) {
    int index = static_cast<int>(tab);
    delete tabScreens[index];
    tabScreens[index] = screen;
    if (tabScreens[index] != nullptr) {
        tabScreens[index]->onEnter();
    }
}

void MainShellScreen::setTabScreen(int tabIndex, Screen* screen) {
    if (tabIndex >= 0 && tabIndex < 5) {
        setTabScreen(static_cast<ShellTab>(tabIndex), screen);
    }
}

void MainShellScreen::setActiveTab(ShellTab tab) {
    if (activeTab == tab && detailCount == 0) return;

    clearDetailStack();
    activeTab = tab;

    Screen* current = tabScreens[static_cast<int>(activeTab)];
    if (current != nullptr) {
        current->onEnter();
    }

    const char* tabNames[5] = { "Home", "Explore", "New Post", "Notifications", "Profile" };
    Logger::log("Tab switched to " + std::string(tabNames[static_cast<int>(tab)]));
}

ShellTab MainShellScreen::getActiveTab() const {
    return activeTab;
}

void MainShellScreen::pushDetailScreen(Screen* screen) {
    if (screen == nullptr) return;

    if (detailCount < MAX_DETAIL_SCREENS) {
        detailStack[detailCount] = screen;
        detailCount++;
        screen->onEnter();
    }
    else {
        delete screen;
    }
}

void MainShellScreen::popDetailScreen() {
    if (detailCount > 0) {
        detailCount--;
        if (detailStack[detailCount] != nullptr) {
            pendingDeletes.push_back(detailStack[detailCount]);
            detailStack[detailCount] = nullptr;
        }
        if (detailCount > 0 && detailStack[detailCount - 1] != nullptr) {
            detailStack[detailCount - 1]->onEnter();
        }
        else {
            Screen* current = tabScreens[static_cast<int>(activeTab)];
            if (current != nullptr) {
                current->onEnter();
            }
        }
    }
}

void MainShellScreen::clearDetailStack() {
    while (detailCount > 0) {
        popDetailScreen();
    }
}

void MainShellScreen::openPostDetail(int postId) {
    int viewerId = (currentUser != nullptr) ? currentUser->getId() : -1;
    pushDetailScreen(new PostDetailScreen(
        postId,
        postManager,
        userManager,
        viewerId,
        [this]() { popDetailScreen(); },
        [this](int authorId) { openProfile(authorId); }
    ));
}

void MainShellScreen::openProfile(int userId) {
    int viewerId = (currentUser != nullptr) ? currentUser->getId() : -1;
    pushDetailScreen(new ProfileScreen(
        userId,
        userManager,
        postManager,
        socialGraph,
        viewerId,
        [this]() { popDetailScreen(); },
        [this](int uid, PeopleListMode mode) { openPeopleList(uid, mode); },
        [this](int pid) { openPostDetail(pid); },
        [this]() { pushDetailScreen(new SettingsScreen(contentBounds, currentUsername)); }
    ));
}

void MainShellScreen::openPeopleList(int userId, PeopleListMode mode) {
    int viewerId = (currentUser != nullptr) ? currentUser->getId() : -1;
    pushDetailScreen(new PeopleListScreen(
        userId,
        mode,
        userManager,
        socialGraph,
        viewerId,
        [this]() { popDetailScreen(); },
        [this](int selectedUserId) { openProfile(selectedUserId); }
    ));
}

Rectangle MainShellScreen::getContentBounds() const {
    return contentBounds;
}

void MainShellScreen::showToast(const std::string& msg, ToastType type) {
    toast.show(msg, type);
}

void MainShellScreen::updateLayout() {
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());

    float frameW = (screenW < 480.0f) ? screenW : 480.0f;
    float frameX = (screenW - frameW) / 2.0f;
    if (frameX < 0.0f) frameX = 0.0f;

    float topH = 56.0f;
    float bottomH = 60.0f;

    frameBounds = { frameX, 0.0f, frameW, screenH };
    topBarBounds = { frameX, 0.0f, frameW, topH };
    bottomBarBounds = { frameX, screenH - bottomH, frameW, bottomH };
    contentBounds = { frameX, topH, frameW, screenH - topH - bottomH };

    float topBtnH = 34.0f;
    float topBtnY = (topH - topBtnH) / 2.0f;
    campusBtnBounds = { frameX + frameW - 122.0f - 12.0f, topBtnY, 122.0f, topBtnH };
    chatBtnBounds = { campusBtnBounds.x - 64.0f - 8.0f, topBtnY, 64.0f, topBtnH };

    float tabW = frameW / 5.0f;
    for (int i = 0; i < 5; i++) {
        tabBtnBounds[i] = { frameX + (tabW * i), bottomBarBounds.y, tabW, bottomH };
    }
}

void MainShellScreen::update() {
    updateLayout();
    if (postManager != nullptr) {
        postManager->purgeExpired();
    }
    toast.update();

    if (modal.isOpen()) {
        modal.update();
        flushPendingDeletes();
        return;
    }

    if (detailCount == 0 && IsKeyPressed(KEY_S)) {
        pushDetailScreen(new SettingsScreen(contentBounds, currentUsername));
        flushPendingDeletes();
        return;
    }

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, chatBtnBounds)) {
            Logger::log("Opened Chat");
            toast.show("Chat screen coming soon", ToastType::Info);
        }
        else if (CheckCollisionPointRec(mouse, campusBtnBounds)) {
            Logger::log("Opened Campus Board");
            toast.show("Campus Board coming soon", ToastType::Info);
        }

        for (int i = 0; i < 5; i++) {
            if (CheckCollisionPointRec(mouse, tabBtnBounds[i])) {
                setActiveTab(static_cast<ShellTab>(i));
                if (i == 4 && tabScreens[4] == nullptr) {
                    pushDetailScreen(new SettingsScreen(contentBounds, currentUsername));
                }
                break;
            }
        }
    }

    if (detailCount > 0) {
        detailStack[detailCount - 1]->update();

        if (detailCount > 0 && detailStack[detailCount - 1] != nullptr) {
            SettingsScreen* settings = dynamic_cast<SettingsScreen*>(detailStack[detailCount - 1]);
            if (settings != nullptr) {
                if (settings->isBackRequested()) {
                    popDetailScreen();
                    flushPendingDeletes();
                    return;
                }
                if (settings->isLogoutRequested() && screenManager != nullptr) {
                    screenManager->setScreen(new LoginScreen(screenManager, userManager, fileManager, GetScreenWidth(), GetScreenHeight()));
                    return;
                }
            }
        }
    }
    else {
        Screen* current = tabScreens[static_cast<int>(activeTab)];
        if (current != nullptr) {
            current->update();
        }
    }

    flushPendingDeletes();
}

void MainShellScreen::drawTopBar() const {
    DrawRectangleRec(topBarBounds, Theme::surface());
    DrawLineEx({ topBarBounds.x, topBarBounds.height }, { topBarBounds.x + topBarBounds.width, topBarBounds.height }, 1.0f, Theme::border());

    DrawTextEx(Theme::appFont, "FASTgram", { topBarBounds.x + 16.0f, 15.0f }, 24.0f, 1.0f, Theme::accent());

    Vector2 mouse = GetMousePosition();

    Color chatBg = CheckCollisionPointRec(mouse, chatBtnBounds) ? Theme::surfaceHover() : Theme::background();
    DrawRectangleRounded(chatBtnBounds, 0.25f, 8, chatBg);
    DrawRectangleRoundedLinesEx(chatBtnBounds, 0.25f, 8, 1.0f, Theme::border());
    Vector2 chatSize = MeasureTextEx(Theme::appFont, "Chat", 14.0f, 1.0f);
    DrawTextEx(Theme::appFont, "Chat", { chatBtnBounds.x + (chatBtnBounds.width - chatSize.x) / 2.0f, chatBtnBounds.y + (chatBtnBounds.height - chatSize.y) / 2.0f }, 14.0f, 1.0f, Theme::primaryText());

    Color campusBg = CheckCollisionPointRec(mouse, campusBtnBounds) ? Theme::surfaceHover() : Theme::background();
    DrawRectangleRounded(campusBtnBounds, 0.25f, 8, campusBg);
    DrawRectangleRoundedLinesEx(campusBtnBounds, 0.25f, 8, 1.0f, Theme::border());
    Vector2 campusSize = MeasureTextEx(Theme::appFont, "Campus Board", 14.0f, 1.0f);
    DrawTextEx(Theme::appFont, "Campus Board", { campusBtnBounds.x + (campusBtnBounds.width - campusSize.x) / 2.0f, campusBtnBounds.y + (campusBtnBounds.height - campusSize.y) / 2.0f }, 14.0f, 1.0f, Theme::primaryText());
}

void MainShellScreen::drawBottomBar() const {
    DrawRectangleRec(bottomBarBounds, Theme::surface());
    DrawLineEx({ bottomBarBounds.x, bottomBarBounds.y }, { bottomBarBounds.x + bottomBarBounds.width, bottomBarBounds.y }, 1.0f, Theme::border());

    const char* labels[5] = { "Home", "Explore", "New Post", "Alerts", "Profile" };
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < 5; i++) {
        bool isActive = (static_cast<int>(activeTab) == i) && (detailCount == 0);
        bool isHovered = CheckCollisionPointRec(mouse, tabBtnBounds[i]);

        if (isHovered && !isActive) {
            DrawRectangleRec(tabBtnBounds[i], Theme::surfaceHover());
        }

        Color textColor = isActive ? Theme::accent() : Theme::secondaryText();
        Vector2 textSize = MeasureTextEx(Theme::appFont, labels[i], 14.0f, 1.0f);
        Vector2 textPos = {
            tabBtnBounds[i].x + (tabBtnBounds[i].width - textSize.x) / 2.0f,
            tabBtnBounds[i].y + (tabBtnBounds[i].height - textSize.y) / 2.0f
        };

        DrawTextEx(Theme::appFont, labels[i], textPos, 14.0f, 1.0f, textColor);

        if (isActive) {
            DrawRectangleRec({ tabBtnBounds[i].x + 18.0f, tabBtnBounds[i].y, tabBtnBounds[i].width - 36.0f, 3.0f }, Theme::accent());
        }
    }
}

void MainShellScreen::draw() const {
    ClearBackground(Theme::outerBackground());

    DrawRectangleRec(frameBounds, Theme::background());
    DrawRectangleLinesEx(frameBounds, 1.0f, Theme::border());

    if (detailCount > 0) {
        detailStack[detailCount - 1]->draw();
    }
    else {
        Screen* current = tabScreens[static_cast<int>(activeTab)];
        if (current != nullptr) {
            current->draw();
        }
    }

    drawTopBar();
    drawBottomBar();

    toast.draw();
    modal.draw();
}