#include "PeopleListScreen.h"
#include "../../core/Theme.h"
#include <algorithm>
#include <cctype>

PeopleListScreen::PeopleListScreen(int userId,
    PeopleListMode listMode,
    UserManager* users,
    SocialGraph* graph,
    int currentViewerId,
    std::function<void()> onBackCallback,
    std::function<void(int)> onSelectCallback)
    : targetUserId(userId),
    viewerUserId(currentViewerId),
    mode(listMode),
    userManager(users),
    socialGraph(graph),
    onBack(onBackCallback),
    onUserSelected(onSelectCallback),
    backBtn({ 0, 0, 74, 34 }, "< Back", ButtonStyle::Secondary, 15),
    searchBox({ 0, 0, 448, 38 }, "Search people...", false, 50, 15),
    scrollView({ 0, 160, 480, 560 }, 35.0f) {
    updateLayout();
    refreshList();
}

void PeopleListScreen::setViewerId(int currentViewerId) {
    viewerUserId = currentViewerId;
}

void PeopleListScreen::setOnBack(std::function<void()> callback) {
    onBack = callback;
}

void PeopleListScreen::setOnUserSelected(std::function<void(int)> callback) {
    onUserSelected = callback;
}

Rectangle PeopleListScreen::getContentArea() const {
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

std::string PeopleListScreen::makeInitials(const std::string& name) const {
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

void PeopleListScreen::updateLayout() {
    Rectangle area = getContentArea();
    float pad = 16.0f;
    float x = area.x + pad;
    float y = area.y + 12.0f;

    backBtn.setBounds({ x, y, 74.0f, 34.0f });
    y += 44.0f;

    searchBox.setBounds({ x, y, area.width - pad * 2.0f, 38.0f });
    y += 48.0f;

    float listH = (area.y + area.height) - y;
    if (listH < 40.0f) listH = 40.0f;
    scrollView.setBounds({ area.x, y, area.width, listH });
}

void PeopleListScreen::refreshList() {
    filteredUserIds.clear();
    if (!userManager || !socialGraph) return;

    std::vector<int> rawIds = (mode == PeopleListMode::Followers)
        ? socialGraph->getFollowers(targetUserId)
        : socialGraph->getFollowing(targetUserId);

    std::string query = searchBox.getText();
    std::transform(query.begin(), query.end(), query.begin(), [](unsigned char c) {
        return (char)std::tolower(c);
        });

    for (int id : rawIds) {
        Account* acc = userManager->findById(id);
        if (!acc) continue;

        if (viewerUserId != -1 && (socialGraph->isBlocked(viewerUserId, id) || socialGraph->isBlocked(id, viewerUserId))) {
            continue;
        }

        if (!query.empty()) {
            std::string disp = acc->getDisplayName();
            std::string user = acc->getUsername();
            std::transform(disp.begin(), disp.end(), disp.begin(), [](unsigned char c) {
                return (char)std::tolower(c);
                });
            std::transform(user.begin(), user.end(), user.begin(), [](unsigned char c) {
                return (char)std::tolower(c);
                });
            if (disp.find(query) == std::string::npos && user.find(query) == std::string::npos) {
                continue;
            }
        }

        filteredUserIds.push_back(id);
    }
}

void PeopleListScreen::onEnter() {
    scrollView.resetScroll();
    updateLayout();
    refreshList();
}

void PeopleListScreen::update() {
    if (!userManager || !userManager->findById(targetUserId)) {
        if (onBack) onBack();
        return;
    }

    updateLayout();

    if (IsKeyPressed(KEY_ESCAPE) && !searchBox.isFocused()) {
        if (onBack) onBack();
        return;
    }

    backBtn.update();
    if (backBtn.isClicked()) {
        if (onBack) onBack();
        return;
    }

    searchBox.update();
    refreshList();

    int count = (int)filteredUserIds.size();
    float totalH = count * ROW_HEIGHT + (count > 0 ? (count - 1) * ROW_GAP : 0.0f);
    scrollView.setContentHeight(totalH);
    scrollView.update();

    Rectangle listArea = scrollView.getBounds();
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, listArea) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        float offset = scrollView.getScrollOffset();
        float pad = 16.0f;

        for (int i = 0; i < count; i++) {
            int personId = filteredUserIds[i];
            Account* person = userManager->findById(personId);
            if (!person) continue;

            Rectangle rowRect = {
                listArea.x + pad,
                listArea.y + i * (ROW_HEIGHT + ROW_GAP) - offset,
                listArea.width - pad * 2.0f,
                ROW_HEIGHT
            };

            if (!CheckCollisionPointRec(mouse, rowRect)) continue;

            Rectangle actionRect = {
                rowRect.x + rowRect.width - 104.0f,
                rowRect.y + (ROW_HEIGHT - 32.0f) / 2.0f,
                92.0f,
                32.0f
            };

            if (viewerUserId != -1 && viewerUserId != personId && socialGraph && CheckCollisionPointRec(mouse, actionRect)) {
                if (socialGraph->isFollowing(viewerUserId, personId)) {
                    socialGraph->unfollow(viewerUserId, personId);
                }
                else if (!socialGraph->hasPendingRequest(viewerUserId, personId)) {
                    socialGraph->follow(viewerUserId, personId, person->getIsPrivate());
                }
            }
            else if (onUserSelected) {
                onUserSelected(personId);
            }
            break;
        }
    }
}

void PeopleListScreen::draw() const {
    Rectangle area = getContentArea();
    DrawRectangleRec(area, Theme::background());

    backBtn.draw();

    const char* title = (mode == PeopleListMode::Followers) ? "Followers" : "Following";
    Rectangle backBounds = backBtn.getBounds();
    DrawTextEx(Theme::appFont, title, { backBounds.x + backBounds.width + 14.0f, backBounds.y + 6.0f }, 20.0f, 1.0f, Theme::primaryText());

    searchBox.draw();

    Rectangle listArea = scrollView.getBounds();
    if (filteredUserIds.empty()) {
        const char* emptyMsg = (mode == PeopleListMode::Followers) ? "No followers found" : "Not following anyone yet";
        Vector2 msgSize = MeasureTextEx(Theme::appFont, emptyMsg, 15.0f, 1.0f);
        DrawTextEx(Theme::appFont,
            emptyMsg,
            { listArea.x + (listArea.width - msgSize.x) / 2.0f, listArea.y + 40.0f },
            15.0f,
            1.0f,
            Theme::secondaryText());
        return;
    }

    float offset = scrollView.getScrollOffset();
    float pad = 16.0f;
    Vector2 mouse = GetMousePosition();
    bool mouseInList = CheckCollisionPointRec(mouse, listArea);

    scrollView.beginClip();

    for (int i = 0; i < (int)filteredUserIds.size(); i++) {
        int personId = filteredUserIds[i];
        Account* person = userManager ? userManager->findById(personId) : nullptr;
        if (!person) continue;

        Rectangle rowRect = {
            listArea.x + pad,
            listArea.y + i * (ROW_HEIGHT + ROW_GAP) - offset,
            listArea.width - pad * 2.0f,
            ROW_HEIGHT
        };

        if (rowRect.y + rowRect.height < listArea.y || rowRect.y > listArea.y + listArea.height) {
            continue;
        }

        bool rowHovered = mouseInList && CheckCollisionPointRec(mouse, rowRect);
        DrawRectangleRounded(rowRect, 0.18f, 8, rowHovered ? Theme::surfaceHover() : Theme::surface());
        DrawRectangleRoundedLinesEx(rowRect, 0.18f, 8, 1.0f, Theme::border());

        Avatar avatar({ rowRect.x + 12.0f, rowRect.y + 11.0f, 42.0f, 42.0f }, makeInitials(person->getDisplayName()));
        avatar.draw();

        float textX = rowRect.x + 64.0f;
        DrawTextEx(Theme::appFont, person->getDisplayName().c_str(), { textX, rowRect.y + 12.0f }, 16.0f, 1.0f, Theme::primaryText());
        std::string handle = "@" + person->getUsername();
        DrawTextEx(Theme::appFont, handle.c_str(), { textX, rowRect.y + 34.0f }, 13.0f, 1.0f, Theme::secondaryText());

        if (viewerUserId != -1 && viewerUserId != personId && socialGraph) {
            bool following = socialGraph->isFollowing(viewerUserId, personId);
            bool requested = socialGraph->hasPendingRequest(viewerUserId, personId);
            const char* btnLabel = following ? "Following" : (requested ? "Requested" : "Follow");

            Rectangle actionRect = {
                rowRect.x + rowRect.width - 104.0f,
                rowRect.y + (ROW_HEIGHT - 32.0f) / 2.0f,
                92.0f,
                32.0f
            };
            bool btnHovered = mouseInList && CheckCollisionPointRec(mouse, actionRect);
            Color btnBg = (following || requested)
                ? (btnHovered ? Theme::surfaceHover() : Theme::background())
                : Theme::accent();
            Color btnText = (following || requested)
                ? Theme::primaryText()
                : (Theme::isDark() ? Theme::background() : WHITE);

            DrawRectangleRounded(actionRect, 0.25f, 8, btnBg);
            if (following || requested) {
                DrawRectangleRoundedLinesEx(actionRect, 0.25f, 8, 1.0f, Theme::border());
            }

            Vector2 lblSize = MeasureTextEx(Theme::appFont, btnLabel, 13.5f, 1.0f);
            DrawTextEx(Theme::appFont,
                btnLabel,
                { actionRect.x + (actionRect.width - lblSize.x) / 2.0f,
                  actionRect.y + (actionRect.height - lblSize.y) / 2.0f },
                13.5f,
                1.0f,
                btnText);
        }
    }

    scrollView.endClip();
    scrollView.draw();
}