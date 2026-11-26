#include "HomeScreen.h"
#include "raylib.h"

// Models
#include "../../models/Post.h"
#include "../../models/Account.h"
#include "../../models/SocialGraph.h"

// Services
#include "../../services/UserManager.h"
#include "../../services/ImageStore.h"
// #include "../../services/PostManager.h" // Uncomment when PostManager.h is ready

// UI / Core / Widgets
#include "../../core/Theme.h"
#include "../Widgets/Avatar.h"
#include "MainShellScreen.h"

HomeScreen::HomeScreen(MainShellScreen* shell,
    PostManager* postMgr,
    SocialGraph* graph,
    ImageStore* imgStore,
    Account* user,
    UserManager* userMgr)
    : followingSorter(graph, user != nullptr ? user->getId() : -1),
    feedScroll(shell != nullptr ? shell->getContentBounds() : Rectangle{ 0, 60, 600, 680 }) {
    mainShell = shell;
    postManager = postMgr;
    socialGraph = graph;
    imageStore = imgStore;
    currentUser = user;
    userManager = userMgr;

    activeSorter = &newestSorter;
    activeSortIndex = 0;

    visiblePostCount = 0;
    for (int i = 0; i < MAX_FEED_POSTS; i++) {
        visiblePosts[i] = nullptr;
    }

    lastClickedPostId = -1;
    lastClickTime = 0.0;

    rebuildFeed();
}

void HomeScreen::rebuildFeed() {
    // 1. Reset cached visible posts
    visiblePostCount = 0;
    for (int i = 0; i < MAX_FEED_POSTS; i++) {
        visiblePosts[i] = nullptr;
    }

    if (postManager == nullptr || activeSorter == nullptr || currentUser == nullptr) {
        return;
    }

    int viewerId = currentUser->getId();

    // Fetch posts from PostManager PostManager.h is not written yet so will uncomment later
    /*
    int totalPosts = postManager->getPostCount();
    for (int i = 0; i < totalPosts; i++) {
        Post* post = postManager->getPostByIndex(i);
        if (post == nullptr) {
            continue;
        }

        int authorId = post->getAuthorId();
        bool isOwnerPrivate = false;
        if (userManager != nullptr) {
            Account* author = userManager->findById(authorId);
            if (author != nullptr) {
                isOwnerPrivate = author->getIsPrivate();
            }
        }

        bool canView = true;
        if (socialGraph != nullptr) {
            canView = socialGraph->canViewContent(viewerId, authorId, isOwnerPrivate)
                      && !socialGraph->isMuted(viewerId, authorId);
        }

        if (canView && activeSorter->shouldInclude(post)) {
            if (visiblePostCount < MAX_FEED_POSTS) {
                visiblePosts[visiblePostCount] = post;
                visiblePostCount++;
            }
        }
    }
    */
    (void)viewerId;

    // Sort visiblePosts using the active FeedSorter functor
    for (int i = 0; i < visiblePostCount - 1; i++) {
        for (int j = 0; j < visiblePostCount - i - 1; j++) {
            if ((*activeSorter)(visiblePosts[j + 1], visiblePosts[j])) {
                Post* temp = visiblePosts[j];
                visiblePosts[j] = visiblePosts[j + 1];
                visiblePosts[j + 1] = temp;
            }
        }
    }

    // Recalculate total content height and update ScrollView bounds
    float cardWidth = (mainShell != nullptr) ? (mainShell->getContentBounds().width - 40.0f) : 560.0f;
    float cardSpacing = 12.0f;
    float totalHeight = 0.0f;

    for (int i = 0; i < visiblePostCount; i++) {
        totalHeight += calculateCardHeight(visiblePosts[i], cardWidth) + cardSpacing;
    }

    feedScroll.setContentHeight(totalHeight);
    feedScroll.resetScroll();
}

void HomeScreen::setSortMode(int index) {
    if (index < 0 || index > 2) {
        return;
    }

    if (activeSortIndex == index) {
        return;
    }

    activeSortIndex = index;

    if (index == 0) {
        activeSorter = &newestSorter;
    }
    else if (index == 1) {
        activeSorter = &mostLikedSorter;
    }
    else if (index == 2) {
        activeSorter = &followingSorter;
    }

    rebuildFeed();
}

void HomeScreen::handleSortBarInput(Rectangle bounds) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mousePos = GetMousePosition();
    if (!CheckCollisionPointRec(mousePos, bounds)) {
        return;
    }

    const int tabCount = 3;
    float paddingX = 12.0f;
    float gap = 8.0f;
    float totalAvailableWidth = bounds.width - (paddingX * 2.0f) - (gap * (tabCount - 1));
    float tabWidth = totalAvailableWidth / static_cast<float>(tabCount);
    float tabHeight = bounds.height - 12.0f;
    float tabY = bounds.y + 6.0f;

    for (int i = 0; i < tabCount; i++) {
        float tabX = bounds.x + paddingX + i * (tabWidth + gap);
        Rectangle tabRect = { tabX, tabY, tabWidth, tabHeight };

        if (CheckCollisionPointRec(mousePos, tabRect)) {
            setSortMode(i);
            break;
        }
    }
}

void HomeScreen::handlePostCardInput(Post* post, Rectangle cardRect) {
    if (post == nullptr || currentUser == nullptr) {
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mousePos = GetMousePosition();
    if (!CheckCollisionPointRec(mousePos, cardRect)) {
        return;
    }

    int currentUserId = currentUser->getId();
    int postId = post->getId();

    float footerHeight = 36.0f;
    Rectangle likeBtnRect = {
        cardRect.x + 12.0f,
        cardRect.y + cardRect.height - footerHeight + 4.0f,
        72.0f,
        28.0f
    };

    // Toggle like when clicking the like button
    if (CheckCollisionPointRec(mousePos, likeBtnRect)) {
        post->toggleLike(currentUserId);
        return;
    }

    // Double-click anywhere on the card to like
    double now = GetTime();
    if (lastClickedPostId == postId && (now - lastClickTime) <= 0.30) {
        if (!post->isLikedBy(currentUserId)) {
            post->toggleLike(currentUserId);
        }
        lastClickedPostId = -1;
        lastClickTime = 0.0;
        return;
    }

    lastClickedPostId = postId;
    lastClickTime = now;

    // Open full post view on single click
    if (mainShell != nullptr) {
        // mainShell->pushDetailScreen(new PostDetailScreen(mainShell, post, currentUser, userManager, socialGraph, imageStore));
    }
}

void HomeScreen::drawSortBar(Rectangle bounds) const {
    const char* labels[3] = { "Newest", "Most Liked", "Following" };
    const int tabCount = 3;

    // Draw sort bar background and bottom divider
    DrawRectangleRec(bounds, { 24, 24, 28, 255 });
    DrawLine(static_cast<int>(bounds.x),
        static_cast<int>(bounds.y + bounds.height),
        static_cast<int>(bounds.x + bounds.width),
        static_cast<int>(bounds.y + bounds.height),
        { 45, 45, 52, 255 });

    float paddingX = 12.0f;
    float gap = 8.0f;
    float totalAvailableWidth = bounds.width - (paddingX * 2.0f) - (gap * (tabCount - 1));
    float tabWidth = totalAvailableWidth / static_cast<float>(tabCount);
    float tabHeight = bounds.height - 12.0f;
    float tabY = bounds.y + 6.0f;
    int fontSize = 15;

    for (int i = 0; i < tabCount; i++) {
        float tabX = bounds.x + paddingX + i * (tabWidth + gap);
        Rectangle tabRect = { tabX, tabY, tabWidth, tabHeight };

        bool isSelected = (activeSortIndex == i);
        Color pillColor = isSelected ? Color{ 70, 130, 255, 255 } : Color{ 38, 38, 44, 255 };
        Color textColor = isSelected ? WHITE : Color{ 175, 175, 185, 255 };

        DrawRectangleRounded(tabRect, 0.35f, 8, pillColor);

        int textWidth = MeasureText(labels[i], fontSize);
        int textX = static_cast<int>(tabRect.x + (tabRect.width - textWidth) * 0.5f);
        int textY = static_cast<int>(tabRect.y + (tabRect.height - fontSize) * 0.5f);

        DrawText(labels[i], textX, textY, fontSize, textColor);
    }
}

void HomeScreen::drawPostCard(const Post* post, Rectangle cardRect) const {
    if (post == nullptr) {
        return;
    }

    // Card background and border
    DrawRectangleRounded(cardRect, 0.04f, 8, { 28, 28, 34, 255 });
    DrawRectangleRoundedLinesEx(cardRect, 0.04f, 8, 1.0f, { 50, 50, 58, 255 });

    float pad = 12.0f;
    float headerHeight = 40.0f;
    float footerHeight = 36.0f;

    // Resolve author info via UserManager
    std::string authorName = "Unknown";
    std::string badgeLabel = "";
    if (userManager != nullptr) {
        Account* author = userManager->findById(post->getAuthorId());
        if (author != nullptr) {
            authorName = author->getDisplayName();
            badgeLabel = author->getBadgeLabel();
        }
    }

    // Author avatar circle + display name
    float avatarRadius = 15.0f;
    Vector2 avatarCenter = { cardRect.x + pad + avatarRadius, cardRect.y + pad + avatarRadius };
    DrawCircleV(avatarCenter, avatarRadius, { 70, 130, 255, 255 });
    if (!authorName.empty()) {
        char initial[2] = { authorName[0], '\0' };
        int initW = MeasureText(initial, 16);
        DrawText(initial, static_cast<int>(avatarCenter.x - initW * 0.5f), static_cast<int>(avatarCenter.y - 8), 16, WHITE);
    }

    int nameX = static_cast<int>(cardRect.x + pad + avatarRadius * 2.0f + 10.0f);
    int nameY = static_cast<int>(cardRect.y + pad + 6.0f);
    DrawText(authorName.c_str(), nameX, nameY, 16, WHITE);

    if (!badgeLabel.empty()) {
        int badgeX = nameX + MeasureText(authorName.c_str(), 16) + 8;
        DrawText(badgeLabel.c_str(), badgeX, nameY + 2, 13, { 130, 180, 255, 255 });
    }

    if (post->getIsPinned()) {
        const char* pinText = "PINNED";
        int pinW = MeasureText(pinText, 12);
        DrawText(pinText, static_cast<int>(cardRect.x + cardRect.width - pad - pinW), nameY + 2, 12, { 255, 200, 90, 255 });
    }

    // Post caption body
    float bodyY = cardRect.y + pad + headerHeight;
    DrawText(post->getCaption().c_str(),
        static_cast<int>(cardRect.x + pad),
        static_cast<int>(bodyY),
        15,
        { 225, 225, 232, 255 });

    // Footer divider and Like button (matches handlePostCardInput geometry)
    float footerY = cardRect.y + cardRect.height - footerHeight;
    DrawLine(static_cast<int>(cardRect.x + pad),
        static_cast<int>(footerY),
        static_cast<int>(cardRect.x + cardRect.width - pad),
        static_cast<int>(footerY),
        { 45, 45, 52, 255 });

    Rectangle likeBtnRect = {
        cardRect.x + 12.0f,
        footerY + 4.0f,
        72.0f,
        28.0f
    };

    bool liked = (currentUser != nullptr) && post->isLikedBy(currentUser->getId());
    Color likeBg = liked ? Color{ 220, 65, 85, 255 } : Color{ 42, 42, 50, 255 };
    DrawRectangleRounded(likeBtnRect, 0.35f, 6, likeBg);

    std::string likeLabel = (liked ? "Liked " : "Like ") + std::to_string(post->getLikeCount());
    int likeTextW = MeasureText(likeLabel.c_str(), 13);
    DrawText(likeLabel.c_str(),
        static_cast<int>(likeBtnRect.x + (likeBtnRect.width - likeTextW) * 0.5f),
        static_cast<int>(likeBtnRect.y + 7.0f),
        13,
        WHITE);
}

float HomeScreen::calculateCardHeight(const Post* post, float cardWidth) const {
    if (post == nullptr) {
        return 0.0f;
    }

    float pad = 12.0f;
    float headerHeight = 40.0f;
    float footerHeight = 36.0f;
    int fontSize = 15;
    float lineHeight = 20.0f;

    float usableWidth = cardWidth - (pad * 2.0f);
    if (usableWidth < 40.0f) {
        usableWidth = 40.0f;
    }

    // Estimate wrapped line count from caption text width and explicit newlines
    const std::string& caption = post->getCaption();
    int lineCount = 1;
    std::string currentLine = "";

    for (size_t i = 0; i < caption.size(); i++) {
        char c = caption[i];
        if (c == '\n') {
            lineCount++;
            currentLine.clear();
            continue;
        }

        currentLine += c;
        if (static_cast<float>(MeasureText(currentLine.c_str(), fontSize)) > usableWidth) {
            lineCount++;
            currentLine.clear();
            currentLine += c;
        }
    }

    float captionHeight = static_cast<float>(lineCount) * lineHeight;
    float mediaHeight = 0.0f;

    // Reserve extra height if the post includes an image
    PostType type = post->getPostType();
    if (type == PostType::Photo || type == PostType::Carousel) {
        mediaHeight = 200.0f;
    }

    return pad + headerHeight + captionHeight + mediaHeight + pad + footerHeight;
}

void HomeScreen::onEnter() {
    // Sync ScrollView viewport below the 48px sort bar
    if (mainShell != nullptr) {
        Rectangle content = mainShell->getContentBounds();
        float sortBarHeight = 48.0f;
        Rectangle scrollBounds = {
            content.x,
            content.y + sortBarHeight,
            content.width,
            content.height - sortBarHeight
        };
        feedScroll.setBounds(scrollBounds);
    }

    // Refresh feed whenever navigating back to HomeScreen
    rebuildFeed();
}

void HomeScreen::update() {
    Rectangle content = (mainShell != nullptr)
        ? mainShell->getContentBounds()
        : Rectangle{ 0.0f, 60.0f, 600.0f, 680.0f };

    float sortBarHeight = 48.0f;
    Rectangle sortBarBounds = {
        content.x,
        content.y,
        content.width,
        sortBarHeight
    };

    Rectangle scrollBounds = {
        content.x,
        content.y + sortBarHeight,
        content.width,
        content.height - sortBarHeight
    };

    // Handle sort tab clicks and mouse wheel scrolling
    handleSortBarInput(sortBarBounds);
    feedScroll.update();

    // Check clicks on visible post cards inside the scroll viewport
    Vector2 mousePos = GetMousePosition();
    if (!CheckCollisionPointRec(mousePos, scrollBounds)) {
        return;
    }

    float padX = 20.0f;
    float cardWidth = content.width - (padX * 2.0f);
    float cardSpacing = 12.0f;
    float currentY = scrollBounds.y + 12.0f - feedScroll.getScrollOffset();

    for (int i = 0; i < visiblePostCount; i++) {
        Post* post = visiblePosts[i];
        if (post == nullptr) {
            continue;
        }

        float cardHeight = calculateCardHeight(post, cardWidth);
        Rectangle cardRect = {
            content.x + padX,
            currentY,
            cardWidth,
            cardHeight
        };

        // Only test cards currently overlapping the visible scroll area
        if (cardRect.y + cardRect.height >= scrollBounds.y &&
            cardRect.y <= scrollBounds.y + scrollBounds.height) {
            handlePostCardInput(post, cardRect);
        }

        currentY += cardHeight + cardSpacing;
    }
}

void HomeScreen::draw() const {
    Rectangle content = (mainShell != nullptr)
        ? mainShell->getContentBounds()
        : Rectangle{ 0.0f, 60.0f, 600.0f, 680.0f };

    // Screen background
    DrawRectangleRec(content, { 18, 18, 22, 255 });

    float sortBarHeight = 48.0f;
    Rectangle sortBarBounds = {
        content.x,
        content.y,
        content.width,
        sortBarHeight
    };

    Rectangle scrollBounds = {
        content.x,
        content.y + sortBarHeight,
        content.width,
        content.height - sortBarHeight
    };

    // Clip scrollable feed cards to the viewport below the sort bar
    feedScroll.beginClip();

    if (visiblePostCount == 0) {
        const char* emptyMsg = "No posts to show yet.";
        int fontSize = 16;
        int textW = MeasureText(emptyMsg, fontSize);
        int textX = static_cast<int>(scrollBounds.x + (scrollBounds.width - textW) * 0.5f);
        int textY = static_cast<int>(scrollBounds.y + scrollBounds.height * 0.4f);
        DrawText(emptyMsg, textX, textY, fontSize, { 140, 140, 150, 255 });
    }
    else {
        float padX = 20.0f;
        float cardWidth = content.width - (padX * 2.0f);
        float cardSpacing = 12.0f;
        float currentY = scrollBounds.y + 12.0f - feedScroll.getScrollOffset();

        for (int i = 0; i < visiblePostCount; i++) {
            const Post* post = visiblePosts[i];
            if (post == nullptr) {
                continue;
            }

            float cardHeight = calculateCardHeight(post, cardWidth);
            Rectangle cardRect = {
                content.x + padX,
                currentY,
                cardWidth,
                cardHeight
            };

            // Only draw cards overlapping the visible scroll viewport
            if (cardRect.y + cardRect.height >= scrollBounds.y &&
                cardRect.y <= scrollBounds.y + scrollBounds.height) {
                drawPostCard(post, cardRect);
            }

            currentY += cardHeight + cardSpacing;
        }
    }

    feedScroll.endClip();

    // Draw scrollbar and top sort bar on top of clipped feed
    feedScroll.draw();
    drawSortBar(sortBarBounds);
}