#include "ExploreScreen.h"
#include "../../core/Theme.h"
#include "../../services/ImageStore.h"
#include "../../models/PhotoPost.h"
#include <algorithm>
#include <cmath>
#include <string>

ExploreScreen::ExploreScreen(PostManager* posts,
    UserManager* users,
    SocialGraph* graph,
    Account* viewer,
    std::function<void(Post*)> onSelect)
    : postManager(posts),
    userManager(users),
    socialGraph(graph),
    currentUser(viewer),
    scrollView({ 0, 56, 480, 664 }, 35.0f),
    onPostSelected(onSelect) {
    refreshPosts();
}

void ExploreScreen::setCurrentUser(Account* viewer) {
    currentUser = viewer;
    refreshPosts();
}

void ExploreScreen::setOnPostSelected(std::function<void(Post*)> callback) {
    onPostSelected = callback;
}

Rectangle ExploreScreen::getContentArea() const {
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

void ExploreScreen::refreshPosts() {
    visiblePosts.clear();
    if (!postManager || !userManager) return;

    std::vector<Post*> publicPosts = postManager->getPublicPosts(*userManager);
    int viewerId = currentUser ? currentUser->getId() : -1;

    for (Post* post : publicPosts) {
        if (!post) continue;

        if (socialGraph && viewerId != -1) {
            if (!socialGraph->canViewContent(viewerId, post->getAuthorId(), false)) {
                continue;
            }
        }
        visiblePosts.push_back(post);
    }
}

void ExploreScreen::onEnter() {
    scrollView.resetScroll();
    refreshPosts();
}

void ExploreScreen::update() {
    Rectangle area = getContentArea();
    scrollView.setBounds(area);

    refreshPosts();

    float tileSize = (COLUMN_WIDTH - (COLUMNS - 1) * GAP) / (float)COLUMNS;
    int totalRows = ((int)visiblePosts.size() + COLUMNS - 1) / COLUMNS;
    float totalHeight = totalRows * tileSize + (totalRows > 0 ? (totalRows - 1) * GAP : 0.0f);
    scrollView.setContentHeight(totalHeight);

    scrollView.update();

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, area) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        float offset = scrollView.getScrollOffset();

        for (int i = 0; i < (int)visiblePosts.size(); i++) {
            int row = i / COLUMNS;
            int col = i % COLUMNS;

            Rectangle tileRect = {
                area.x + col * (tileSize + GAP),
                area.y + row * (tileSize + GAP) - offset,
                tileSize,
                tileSize
            };

            if (CheckCollisionPointRec(mouse, tileRect)) {
                if (onPostSelected) {
                    onPostSelected(visiblePosts[i]);
                }
                break;
            }
        }
    }
}

void ExploreScreen::drawTile(Post* post, Rectangle tileRect, bool isHovered) const {
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

void ExploreScreen::draw() const {
    Rectangle area = getContentArea();
    DrawRectangleRec(area, Theme::background());

    if (visiblePosts.empty()) {
        const char* emptyMsg = "No public posts to explore yet";
        Vector2 msgSize = MeasureTextEx(Theme::appFont, emptyMsg, 16.0f, 1.0f);
        DrawTextEx(Theme::appFont,
            emptyMsg,
            { area.x + (area.width - msgSize.x) / 2.0f, area.y + area.height / 2.0f - msgSize.y / 2.0f },
            16.0f,
            1.0f,
            Theme::secondaryText());
        return;
    }

    float tileSize = (COLUMN_WIDTH - (COLUMNS - 1) * GAP) / (float)COLUMNS;
    float offset = scrollView.getScrollOffset();
    Vector2 mouse = GetMousePosition();
    bool mouseInArea = CheckCollisionPointRec(mouse, area);

    scrollView.beginClip();

    for (int i = 0; i < (int)visiblePosts.size(); i++) {
        int row = i / COLUMNS;
        int col = i % COLUMNS;

        Rectangle tileRect = {
            area.x + col * (tileSize + GAP),
            area.y + row * (tileSize + GAP) - offset,
            tileSize,
            tileSize
        };

        if (tileRect.y + tileRect.height < area.y || tileRect.y > area.y + area.height) {
            continue;
        }

        bool isHovered = mouseInArea && CheckCollisionPointRec(mouse, tileRect);
        drawTile(visiblePosts[i], tileRect, isHovered);
    }

    scrollView.endClip();
    scrollView.draw();
}