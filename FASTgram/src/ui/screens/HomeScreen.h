#pragma once

#include <string>
#include "raylib.h"
#include "Screen.h"
#include "../Widgets/ScrollView.h"
#include "../../services/FeedSorter.h"
#include "../../services/NewestFirstSorter.h"
#include "../../services/MostLikedSorter.h"
#include "../../services/FollowingOnlyFilter.h"

// Forward declarations
class Post;
class PostManager;
class SocialGraph;
class ImageStore;
class MainShellScreen;
class Account;
class UserManager;

class HomeScreen : public Screen {
private:
    static const int MAX_FEED_POSTS = 256;

    // Non-owning pointers to shell, user, and services
    MainShellScreen* mainShell;
    PostManager* postManager;
    SocialGraph* socialGraph;
    ImageStore* imageStore;
    Account* currentUser;
    UserManager* userManager;

    // Feed sorting strategies
    NewestFirstSorter newestSorter;
    MostLikedSorter mostLikedSorter;
    FollowingOnlyFilter followingSorter;
    FeedSorter* activeSorter;
    int activeSortIndex; // 0 = Newest, 1 = Most Liked, 2 = Following

    // Cached filtered & sorted feed posts
    Post* visiblePosts[MAX_FEED_POSTS];
    int visiblePostCount;

    // Scrolling & double-click like tracking
    ScrollView feedScroll;
    int lastClickedPostId;
    double lastClickTime;

    // Internal helpers
    void rebuildFeed();
    void setSortMode(int index);
    void handleSortBarInput(Rectangle bounds);
    void handlePostCardInput(Post* post, Rectangle cardRect);
    void drawSortBar(Rectangle bounds) const;
    void drawPostCard(const Post* post, Rectangle cardRect) const;
    float calculateCardHeight(const Post* post, float cardWidth) const;

public:
    HomeScreen(MainShellScreen* shell,
        PostManager* postMgr,
        SocialGraph* graph,
        ImageStore* imgStore,
        Account* user,
        UserManager* userMgr);

    ~HomeScreen() override = default;

    void onEnter();
    void update() override;
    void draw() const override;
};