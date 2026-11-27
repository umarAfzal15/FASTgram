#include "ProfileScreen.h"
#include "../../core/Theme.h"
#include "../../models/PhotoPost.h"
#include <algorithm>
#include <cctype>

ProfileScreen::ProfileScreen(int userId,
    UserManager* users,
    PostManager* posts,
    SocialGraph* graph,
    int currentViewerId,
    std::function<void()> onBackCallback,
    std::function<void(int, PeopleListMode)> onPeopleCallback,
    std::function<void(int)> onPostCallback,
    std::function<void()> onEditCallback)
    : profileUserId(userId),
    viewerUserId(currentViewerId),
    userManager(users),
    postManager(posts),
    socialGraph(graph),
    onBack(onBackCallback),
    onOpenPeopleList(onPeopleCallback),
    onPostSelected(onPostCallback),
    onEditProfile(onEditCallback),
    backBtn({ 0, 0, 74, 34 }, "< Back", ButtonStyle::Secondary, 15),
    followBtn({ 0, 0, 448, 38 }, "Follow", ButtonStyle::Primary, 15),
    editBtn({ 0, 0, 218, 38 }, "Edit Profile", ButtonStyle::Secondary, 15),
    exportBtn({ 0, 0, 218, 38 }, "Export", ButtonStyle::Secondary, 15),
    scrollView({ 0, 56, 480, 664 }, 35.0f),
    postsStatRect{ 0, 0, 0, 0 },
    followersStatRect{ 0, 0, 0, 0 },
    followingStatRect{ 0, 0, 0, 0 } {
    refreshPosts();
    updateLayout(viewerUserId == profileUserId);
}

void ProfileScreen::setViewerId(int currentViewerId) {
    viewerUserId = currentViewerId;
}

void ProfileScreen::setOnBack(std::function<void()> callback) {
    onBack = callback;
}

void ProfileScreen::setOnOpenPeopleList(std::function<void(int, PeopleListMode)> callback) {
    onOpenPeopleList = callback;
}

void ProfileScreen::setOnPostSelected(std::function<void(int)> callback) {
    onPostSelected = callback;
}

void ProfileScreen::setOnEditProfile(std::function<void()> callback) {
    onEditProfile = callback;
}

Rectangle ProfileScreen::getContentArea() const {
    float screenW = (float)Theme::getWindowWidth();
    float screenH = (float)Theme::getWindowHeight();
    float colX = (screenW - COLUMN_WIDTH) / 2.0f;
    if (colX < 0.0f) colX = 0.0f;

    float topBarH = 56.0f;
    float bottomBarH = 56.0f;
    float contentH = screenH - topBarH - bottomBarH;
    if (contentH < 100.0f) contentH = 100.0f;

    return { colX, topBarH, COLUMN_WIDTH, contentH };
}

std::string ProfileScreen::makeInitials(const std::string& name) const {
    if (name.empty()) return "U";
    std::string result;
    result += (char)std::toupper((unsigned char)name[0]);
    for (size_t i = 1; i < name.size(); i++) {
        if (name[i - 1] == ' ' && name[i] != ' ') {
            result += (char)std::toupper((unsigned char)name[i]);
            break;
        }
    }
    return result;
}

bool ProfileScreen::canViewFullProfile(const Account* target) const {
    if (!target) return false;
    if (viewerUserId == profileUserId) return true;
    if (!socialGraph) return !target->getIsPrivate();
    return socialGraph->canViewContent(viewerUserId, profileUserId, target->getIsPrivate());
}

void ProfileScreen::refreshPosts() {
    userPosts.clear();
    if (!postManager) return;
    userPosts = postManager->getPostsByUser(profileUserId);
}

void ProfileScreen::updateLayout(bool isOwnProfile) {
    Rectangle area = getContentArea();
    float pad = 16.0f;
    float innerW = area.width - pad * 2.0f;
    float x = area.x + pad;
    float y = area.y + 12.0f;

    backBtn.setBounds({ x, y, 74.0f, 34.0f });
    y += 46.0f;

    float statX = x + 92.0f;
    float statW = (innerW - 92.0f) / 3.0f;
    postsStatRect = { statX, y + 6.0f, statW, 56.0f };
    followersStatRect = { statX + statW, y + 6.0f, statW, 56.0f };
    followingStatRect = { statX + statW * 2.0f, y + 6.0f, statW, 56.0f };
    y += 126.0f;

    if (isOwnProfile) {
        float halfW = (innerW - 10.0f) / 2.0f;
        editBtn.setBounds({ x, y, halfW, 36.0f });
        exportBtn.setBounds({ x + halfW + 10.0f, y, halfW, 36.0f });
    }
    else {
        followBtn.setBounds({ x, y, innerW, 36.0f });
    }
    y += 48.0f;

    float gridH = (area.y + area.height) - y;
    if (gridH < 40.0f) gridH = 40.0f;
    scrollView.setBounds({ area.x, y, area.width, gridH });
}

void ProfileScreen::onEnter() {
    scrollView.resetScroll();
    refreshPosts();
}

void ProfileScreen::update() {
    Account* target = userManager ? userManager->findById(profileUserId) : nullptr;
    if (!target) {
        if (onBack) onBack();
        return;
    }

    bool isOwnProfile = (viewerUserId != -1 && viewerUserId == profileUserId);
    updateLayout(isOwnProfile);
    refreshPosts();

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (onBack) onBack();
        return;
    }

    backBtn.update();
    if (backBtn.isClicked()) {
        if (onBack) onBack();
        return;
    }

    if (isOwnProfile) {
        editBtn.update();
        exportBtn.update();

        if (editBtn.isClicked() && onEditProfile) {
            onEditProfile();
        }
        if (exportBtn.isClicked()) {
            int postCount = (int)userPosts.size();
            int followers = socialGraph ? socialGraph->getFollowerCount(profileUserId) : 0;
            int following = socialGraph ? socialGraph->getFollowingCount(profileUserId) : 0;
            if (TextFileManager::exportUserSummary(target, postCount, followers, following)) {
                toast.show("Exported data/export_" + target->getUsername() + ".txt", ToastType::Success);
            }
            else {
                toast.show("Failed to export profile summary", ToastType::Error);
            }
        }
    }
    else if (viewerUserId != -1 && socialGraph) {
        bool following = socialGraph->isFollowing(viewerUserId, profileUserId);
        bool requested = socialGraph->hasPendingRequest(viewerUserId, profileUserId);

        if (following) {
            followBtn.setText("Following");
            followBtn.setStyle(ButtonStyle::Secondary);
        }
        else if (requested) {
            followBtn.setText("Requested");
            followBtn.setStyle(ButtonStyle::Secondary);
        }
        else {
            followBtn.setText("Follow");
            followBtn.setStyle(ButtonStyle::Primary);
        }

        followBtn.update();
        if (followBtn.isClicked()) {
            if (following) {
                socialGraph->unfollow(viewerUserId, profileUserId);
            }
            else if (!requested) {
                socialGraph->follow(viewerUserId, profileUserId, target->getIsPrivate());
            }
        }
    }

    bool allowed = canViewFullProfile(target);
    Vector2 mouse = GetMousePosition();

    if (allowed && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, followersStatRect) && onOpenPeopleList) {
            onOpenPeopleList(profileUserId, PeopleListMode::Followers);
            return;
        }
        if (CheckCollisionPointRec(mouse, followingStatRect) && onOpenPeopleList) {
            onOpenPeopleList(profileUserId, PeopleListMode::Following);
            return;
        }
    }

    if (allowed) {
        float tileSize = (COLUMN_WIDTH - (COLUMNS - 1) * GAP) / (float)COLUMNS;
        int totalRows = ((int)userPosts.size() + COLUMNS - 1) / COLUMNS;
        float totalHeight = totalRows * tileSize + (totalRows > 0 ? (totalRows - 1) * GAP : 0.0f);
        scrollView.setContentHeight(totalHeight);
        scrollView.update();

        Rectangle gridArea = scrollView.getBounds();
        if (CheckCollisionPointRec(mouse, gridArea) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float offset = scrollView.getScrollOffset();
            for (int i = 0; i < (int)userPosts.size(); i++) {
                int row = i / COLUMNS;
                int col = i % COLUMNS;
                Rectangle tileRect = {
                    gridArea.x + col * (tileSize + GAP),
                    gridArea.y + row * (tileSize + GAP) - offset,
                    tileSize,
                    tileSize
                };
                if (CheckCollisionPointRec(mouse, tileRect)) {
                    if (onPostSelected && userPosts[i]) {
                        onPostSelected(userPosts[i]->getId());
                    }
                    break;
                }
            }
        }
    }

    toast.update();
}

void ProfileScreen::drawTile(Post* post, Rectangle tileRect, bool isHovered) const {
    PostType type = post->getPostType();

    Color tileBg = Theme::surface();
    const char* typeBadge = "POST";

    if (type == PostType::Photo || type == PostType::Carousel) {
        tileBg = Theme::isDark() ? Color{ 44, 52, 68, 255 } : Color{ 218, 226, 240, 255 };
        typeBadge = (type == PostType::Carousel) ? "ALBUM" : "PHOTO";
    }
    else if (type == PostType::Poll) {
        tileBg = Theme::isDark() ? Color{ 58, 44, 74, 255 } : Color{ 235, 224, 248, 255 };
        typeBadge = "POLL";
    }
    else if (type == PostType::Text) {
        tileBg = Theme::isDark() ? Color{ 38, 60, 56, 255 } : Color{ 220, 242, 236, 255 };
        typeBadge = "TEXT";
    }
    else if (type == PostType::Notice) {
        tileBg = Theme::isDark() ? Color{ 72, 54, 36, 255 } : Color{ 252, 236, 214, 255 };
        typeBadge = "NOTICE";
    }
    else if (type == PostType::LostFound) {
        tileBg = Theme::isDark() ? Color{ 70, 42, 46, 255 } : Color{ 250, 222, 226, 255 };
        typeBadge = "LOST/FOUND";
    }

    DrawRectangleRec(tileRect, tileBg);

    bool drewPhoto = false;
    if (type == PostType::Photo) {
        PhotoPost* photo = dynamic_cast<PhotoPost*>(post);
        if (photo && !photo->getImagePath().empty()) {
            Texture2D tex = ImageStore::getTexture(photo->getImagePath());
            if (tex.id > 0) {
                float side = (float)std::min(tex.width, tex.height);
                Rectangle src = {
                    (tex.width - side) / 2.0f,
                    (tex.height - side) / 2.0f,
                    side,
                    side
                };
                DrawTexturePro(tex, src, tileRect, { 0, 0 }, 0.0f, WHITE);
                drewPhoto = true;
            }
        }
    }

    float badgeFontSize = 11.0f;
    Vector2 badgeTextSize = MeasureTextEx(Theme::appFont, typeBadge, badgeFontSize, 1.0f);
    Rectangle badgeRect = {
        tileRect.x + tileRect.width - badgeTextSize.x - 14.0f,
        tileRect.y + 6.0f,
        badgeTextSize.x + 10.0f,
        badgeTextSize.y + 4.0f
    };
    DrawRectangleRounded(badgeRect, 0.4f, 6, Fade(BLACK, 0.45f));
    DrawTextEx(Theme::appFont, typeBadge, { badgeRect.x + 5.0f, badgeRect.y + 2.0f }, badgeFontSize, 1.0f, WHITE);

    if (!drewPhoto) {
        std::string snippet = post->getCaption();
        if (snippet.empty()) {
            snippet = (type == PostType::Photo || type == PostType::Carousel) ? "[Photo Post]" : "(No caption)";
        }
        else if (snippet.length() > 42) {
            snippet = snippet.substr(0, 39) + "...";
        }

        float textX = tileRect.x + 10.0f;
        float textY = tileRect.y + 34.0f;
        float fontSize = 14.0f;

        std::string line1 = snippet;
        std::string line2 = "";
        if (snippet.length() > 18) {
            size_t split = snippet.rfind(' ', 18);
            if (split == std::string::npos) split = 18;
            line1 = snippet.substr(0, split);
            line2 = snippet.substr(split + (snippet[split] == ' ' ? 1 : 0));
            if (line2.length() > 20) line2 = line2.substr(0, 17) + "...";
        }

        DrawTextEx(Theme::appFont, line1.c_str(), { textX, textY }, fontSize, 1.0f, Theme::primaryText());
        if (!line2.empty()) {
            DrawTextEx(Theme::appFont, line2.c_str(), { textX, textY + 18.0f }, fontSize, 1.0f, Theme::primaryText());
        }
    }

    std::string likesLabel = std::to_string(post->getLikeCount()) + " likes";
    if (drewPhoto) {
        Rectangle footerRect = { tileRect.x, tileRect.y + tileRect.height - 26.0f, tileRect.width, 26.0f };
        DrawRectangleRec(footerRect, Fade(BLACK, 0.45f));
        DrawTextEx(Theme::appFont, likesLabel.c_str(), { tileRect.x + 10.0f, tileRect.y + tileRect.height - 20.0f }, 12.0f, 1.0f, WHITE);
    }
    else {
        DrawTextEx(Theme::appFont, likesLabel.c_str(), { tileRect.x + 10.0f, tileRect.y + tileRect.height - 22.0f }, 12.0f, 1.0f, Theme::secondaryText());
    }

    if (isHovered) {
        DrawRectangleRec(tileRect, Fade(BLACK, 0.18f));
        DrawRectangleLinesEx(tileRect, 2.0f, Theme::accent());
    }
}

void ProfileScreen::draw() const {
    Rectangle area = getContentArea();
    DrawRectangleRec(area, Theme::background());

    Account* target = userManager ? userManager->findById(profileUserId) : nullptr;
    if (!target) return;

    backBtn.draw();

    Rectangle backBounds = backBtn.getBounds();
    std::string headerHandle = "@" + target->getUsername();
    DrawTextEx(Theme::appFont, headerHandle.c_str(), { backBounds.x + backBounds.width + 12.0f, backBounds.y + 6.0f }, 18.0f, 1.0f, Theme::primaryText());

    std::string badge = target->getBadgeLabel();
    if (!badge.empty()) {
        Vector2 hSize = MeasureTextEx(Theme::appFont, headerHandle.c_str(), 18.0f, 1.0f);
        Vector2 bSize = MeasureTextEx(Theme::appFont, badge.c_str(), 12.0f, 1.0f);
        Rectangle badgeRect = {
            backBounds.x + backBounds.width + 20.0f + hSize.x,
            backBounds.y + 6.0f,
            bSize.x + 12.0f,
            22.0f
        };
        DrawRectangleRounded(badgeRect, 0.4f, 6, Theme::surface());
        DrawRectangleRoundedLinesEx(badgeRect, 0.4f, 6, 1.0f, Theme::accent());
        DrawTextEx(Theme::appFont, badge.c_str(), { badgeRect.x + 6.0f, badgeRect.y + 4.0f }, 12.0f, 1.0f, Theme::accent());
    }

    float pad = 16.0f;
    float x = area.x + pad;
    float headerY = backBounds.y + 46.0f;

    Avatar avatar({ x, headerY, 72.0f, 72.0f }, makeInitials(target->getDisplayName()));
    avatar.draw();

    bool isOwnProfile = (viewerUserId != -1 && viewerUserId == profileUserId);
    bool allowed = canViewFullProfile(target);

    if (allowed) {
        int postCount = (int)userPosts.size();
        int followers = socialGraph ? socialGraph->getFollowerCount(profileUserId) : 0;
        int following = socialGraph ? socialGraph->getFollowingCount(profileUserId) : 0;

        auto drawStat = [](Rectangle rect, int count, const char* label) {
            std::string numStr = std::to_string(count);
            Vector2 numSize = MeasureTextEx(Theme::appFont, numStr.c_str(), 18.0f, 1.0f);
            Vector2 lblSize = MeasureTextEx(Theme::appFont, label, 13.0f, 1.0f);
            DrawTextEx(Theme::appFont, numStr.c_str(), { rect.x + (rect.width - numSize.x) / 2.0f, rect.y + 6.0f }, 18.0f, 1.0f, Theme::primaryText());
            DrawTextEx(Theme::appFont, label, { rect.x + (rect.width - lblSize.x) / 2.0f, rect.y + 28.0f }, 13.0f, 1.0f, Theme::secondaryText());
            };

        drawStat(postsStatRect, postCount, "Posts");
        drawStat(followersStatRect, followers, "Followers");
        drawStat(followingStatRect, following, "Following");
    }

    float infoY = headerY + 80.0f;
    DrawTextEx(Theme::appFont, target->getDisplayName().c_str(), { x, infoY }, 17.0f, 1.0f, Theme::primaryText());

    if (allowed) {
        const std::string& bio = target->getBio();
        if (!bio.empty()) {
            std::string shortBio = (bio.size() > 64) ? bio.substr(0, 61) + "..." : bio;
            DrawTextEx(Theme::appFont, shortBio.c_str(), { x, infoY + 22.0f }, 14.0f, 1.0f, Theme::secondaryText());
        }
    }

    if (isOwnProfile) {
        editBtn.draw();
        exportBtn.draw();
    }
    else if (viewerUserId != -1) {
        followBtn.draw();
    }

    Rectangle gridArea = scrollView.getBounds();
    DrawLineEx({ area.x, gridArea.y - 2.0f }, { area.x + area.width, gridArea.y - 2.0f }, 1.0f, Theme::border());

    if (!allowed) {
        const char* lockTitle = "This account is private";
        const char* lockSub = "Follow this account to see their photos and posts.";
        Vector2 tSize = MeasureTextEx(Theme::appFont, lockTitle, 18.0f, 1.0f);
        Vector2 sSize = MeasureTextEx(Theme::appFont, lockSub, 14.0f, 1.0f);
        DrawTextEx(Theme::appFont, lockTitle, { gridArea.x + (gridArea.width - tSize.x) / 2.0f, gridArea.y + 56.0f }, 18.0f, 1.0f, Theme::primaryText());
        DrawTextEx(Theme::appFont, lockSub, { gridArea.x + (gridArea.width - sSize.x) / 2.0f, gridArea.y + 84.0f }, 14.0f, 1.0f, Theme::secondaryText());
        toast.draw();
        return;
    }

    if (userPosts.empty()) {
        const char* emptyMsg = "No posts yet";
        Vector2 msgSize = MeasureTextEx(Theme::appFont, emptyMsg, 15.0f, 1.0f);
        DrawTextEx(Theme::appFont, emptyMsg, { gridArea.x + (gridArea.width - msgSize.x) / 2.0f, gridArea.y + 50.0f }, 15.0f, 1.0f, Theme::secondaryText());
        toast.draw();
        return;
    }

    float tileSize = (COLUMN_WIDTH - (COLUMNS - 1) * GAP) / (float)COLUMNS;
    float offset = scrollView.getScrollOffset();
    Vector2 mouse = GetMousePosition();
    bool mouseInGrid = CheckCollisionPointRec(mouse, gridArea);

    scrollView.beginClip();

    for (int i = 0; i < (int)userPosts.size(); i++) {
        if (!userPosts[i]) continue;
        int row = i / COLUMNS;
        int col = i % COLUMNS;
        Rectangle tileRect = {
            gridArea.x + col * (tileSize + GAP),
            gridArea.y + row * (tileSize + GAP) - offset,
            tileSize,
            tileSize
        };
        if (tileRect.y + tileRect.height < gridArea.y || tileRect.y > gridArea.y + gridArea.height) {
            continue;
        }
        bool isHovered = mouseInGrid && CheckCollisionPointRec(mouse, tileRect);
        drawTile(userPosts[i], tileRect, isHovered);
    }

    scrollView.endClip();
    scrollView.draw();
    toast.draw();
}