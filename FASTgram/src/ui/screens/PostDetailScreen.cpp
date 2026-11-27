#include "PostDetailScreen.h"
#include "../../core/Theme.h"
#include "../../core/Time.h"
#include "../../models/TextFileManager.h"
#include <algorithm>
#include <cctype>

PostDetailScreen::PostDetailScreen(int targetPostId,
    PostManager* posts,
    UserManager* users,
    int currentViewerId,
    std::function<void()> onBackCallback,
    std::function<void(int)> onAuthorCallback)
    : postId(targetPostId),
    viewerUserId(currentViewerId),
    postManager(posts),
    userManager(users),
    onBack(onBackCallback),
    onAuthorSelected(onAuthorCallback),
    backBtn({ 0, 0, 74, 34 }, "< Back", ButtonStyle::Secondary, 15),
    commentBox({ 0, 0, 360, 38 }, "Add a comment...", false, 200, 14),
    sendBtn({ 0, 0, 78, 38 }, "Send", ButtonStyle::Primary, 14),
    scrollView({ 0, 108, 480, 600 }, 35.0f),
    carouselIndex(0),
    isEditingCaption(false),
    editCaptionBox({ 0, 0, 448, 38 }, "Edit caption...", false, 280, 14),
    saveEditBtn({ 0, 0, 80, 32 }, "Save", ButtonStyle::Primary, 14),
    cancelEditBtn({ 0, 0, 80, 32 }, "Cancel", ButtonStyle::Secondary, 14),
    pendingUndoPostId(-1),
    undoTimer(0.0f),
    replyingToCommentId(-1),
    nextCommentId(1),
    authorHitRect{ 0, 0, 0, 0 },
    likeBtnRect{ 0, 0, 0, 0 },
    carouselPrevRect{ 0, 0, 0, 0 },
    carouselNextRect{ 0, 0, 0, 0 },
    editActionRect{ 0, 0, 0, 0 },
    deleteActionRect{ 0, 0, 0, 0 },
    reportActionRect{ 0, 0, 0, 0 },
    undoBtnRect{ 0, 0, 0, 0 } {
    updateLayout();
}

void PostDetailScreen::setViewerId(int currentViewerId) {
    viewerUserId = currentViewerId;
}

void PostDetailScreen::setOnBack(std::function<void()> callback) {
    onBack = callback;
}

void PostDetailScreen::setOnAuthorSelected(std::function<void(int)> callback) {
    onAuthorSelected = callback;
}

Rectangle PostDetailScreen::getContentArea() const {
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

std::string PostDetailScreen::makeInitials(const std::string& name) const {
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

void PostDetailScreen::updateLayout() {
    Rectangle area = getContentArea();
    float pad = 16.0f;
    float x = area.x + pad;

    backBtn.setBounds({ x, area.y + 10.0f, 74.0f, 34.0f });

    float topSectionH = 54.0f;
    float replyBannerH = (replyingToCommentId != -1) ? 24.0f : 0.0f;
    float inputBarH = 54.0f + replyBannerH;

    float scrollY = area.y + topSectionH;
    float scrollH = area.height - topSectionH - inputBarH;
    if (scrollH < 60.0f) scrollH = 60.0f;
    scrollView.setBounds({ area.x, scrollY, area.width, scrollH });

    float inputY = area.y + area.height - 46.0f;
    float sendW = 76.0f;
    float boxW = area.width - pad * 2.0f - sendW - 8.0f;
    commentBox.setBounds({ x, inputY, boxW, 38.0f });
    sendBtn.setBounds({ x + boxW + 8.0f, inputY, sendW, 38.0f });

    undoBtnRect = { area.x + area.width - pad - 76.0f, area.y + 11.0f, 76.0f, 32.0f };
}

void PostDetailScreen::toggleCommentLike(int commentId, int userId) {
    if (userId == -1) return;
    for (CommentNode& node : comments) {
        if (node.id == commentId) {
            auto it = std::find(node.likedBy.begin(), node.likedBy.end(), userId);
            if (it != node.likedBy.end()) {
                node.likedBy.erase(it);
            }
            else {
                node.likedBy.push_back(userId);
            }
            return;
        }
    }
}

int PostDetailScreen::countReplies(int parentId) const {
    int count = 0;
    for (const CommentNode& node : comments) {
        if (node.parentId == parentId) {
            count += 1 + countReplies(node.id);
        }
    }
    return count;
}

float PostDetailScreen::measureCommentTreeHeight(int parentId, int depth, float maxW) const {
    float total = 0.0f;
    for (const CommentNode& node : comments) {
        if (node.parentId != parentId) continue;
        total += 66.0f;
        total += measureCommentTreeHeight(node.id, depth + 1, maxW);
    }
    return total;
}

void PostDetailScreen::drawCommentTree(int parentId, int depth, float startX, float& cursorY, float maxW, float offset, Rectangle listArea, Vector2 mouse) const {
    for (const CommentNode& node : comments) {
        if (node.parentId != parentId) continue;

        float indent = (float)std::min(depth, 4) * 22.0f;
        float rowX = startX + indent;
        float rowW = maxW - indent;
        float rowH = 60.0f;

        Rectangle rowRect = { rowX, cursorY - offset, rowW, rowH };

        if (rowRect.y + rowRect.height >= listArea.y && rowRect.y <= listArea.y + listArea.height) {
            if (depth > 0) {
                DrawLineEx({ rowX - 10.0f, rowRect.y + 6.0f }, { rowX - 10.0f, rowRect.y + rowH - 6.0f }, 2.0f, Theme::border());
            }

            DrawRectangleRounded(rowRect, 0.16f, 6, Theme::surface());
            DrawRectangleRoundedLinesEx(rowRect, 0.16f, 6, 1.0f, Theme::border());

            Account* cAuthor = userManager ? userManager->findById(node.authorId) : nullptr;
            std::string authorName = cAuthor ? ("@" + cAuthor->getUsername()) : "User";

            Avatar cAvatar({ rowRect.x + 8.0f, rowRect.y + 12.0f, 32.0f, 32.0f }, makeInitials(authorName));
            cAvatar.draw();

            float textX = rowRect.x + 48.0f;
            DrawTextEx(Theme::appFont, authorName.c_str(), { textX, rowRect.y + 8.0f }, 13.0f, 1.0f, Theme::primaryText());

            if (!node.timestamp.empty()) {
                Vector2 aSize = MeasureTextEx(Theme::appFont, authorName.c_str(), 13.0f, 1.0f);
                DrawTextEx(Theme::appFont, node.timestamp.c_str(), { textX + aSize.x + 8.0f, rowRect.y + 9.0f }, 11.5f, 1.0f, Theme::secondaryText());
            }

            std::string clippedText = node.text;
            if (clippedText.size() > 46) clippedText = clippedText.substr(0, 43) + "...";
            DrawTextEx(Theme::appFont, clippedText.c_str(), { textX, rowRect.y + 27.0f }, 14.0f, 1.0f, Theme::primaryText());

            bool liked = (viewerUserId != -1) &&
                (std::find(node.likedBy.begin(), node.likedBy.end(), viewerUserId) != node.likedBy.end());
            std::string likeLabel = (liked ? "Liked (" : "Like (") + std::to_string(node.likedBy.size()) + ")";

            Rectangle likeRect = { rowRect.x + rowRect.width - 116.0f, rowRect.y + 6.0f, 62.0f, 20.0f };
            Rectangle replyRect = { rowRect.x + rowRect.width - 50.0f, rowRect.y + 6.0f, 44.0f, 20.0f };

            Color likeColor = liked ? Theme::danger() : Theme::secondaryText();
            DrawTextEx(Theme::appFont, likeLabel.c_str(), { likeRect.x, likeRect.y + 2.0f }, 12.0f, 1.0f, likeColor);
            DrawTextEx(Theme::appFont, "Reply", { replyRect.x, replyRect.y + 2.0f }, 12.0f, 1.0f, Theme::accent());

            commentHitboxes.push_back({ node.id, likeRect, replyRect });
        }

        cursorY += 66.0f;
        drawCommentTree(node.id, depth + 1, startX, cursorY, maxW, offset, listArea, mouse);
    }
}

void PostDetailScreen::onEnter() {
    scrollView.resetScroll();
    carouselIndex = 0;
    isEditingCaption = false;
    replyingToCommentId = -1;
    pendingUndoPostId = -1;
    undoTimer = 0.0f;
    updateLayout();
}

void PostDetailScreen::update() {
    updateLayout();

    if (pendingUndoPostId != -1) {
        undoTimer -= GetFrameTime();
        toast.update();

        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, undoBtnRect)) {
            if (postManager && postManager->undoDelete(pendingUndoPostId)) {
                toast.show("Post restored", ToastType::Success, 2.5f);
            }
            pendingUndoPostId = -1;
            undoTimer = 0.0f;
            return;
        }

        if (undoTimer <= 0.0f) {
            pendingUndoPostId = -1;
            undoTimer = 0.0f;
            if (onBack) onBack();
            return;
        }

        backBtn.update();
        if (backBtn.isClicked() || IsKeyPressed(KEY_ESCAPE)) {
            if (onBack) onBack();
        }
        return;
    }

    Post* post = postManager ? postManager->findById(postId) : nullptr;
    if (!post) {
        if (onBack) onBack();
        return;
    }

    if (reportModal.isOpen()) {
        reportModal.update();
        if (reportModal.isConfirmed()) {
            if (viewerUserId != -1) {
                TextFileManager::appendReport(postId, viewerUserId, "Reported from PostDetailScreen");
                toast.show("Post reported. Thank you.", ToastType::Info);
            }
        }
        toast.update();
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE) && !commentBox.isFocused() && !editCaptionBox.isFocused()) {
        if (replyingToCommentId != -1) {
            replyingToCommentId = -1;
        }
        else if (isEditingCaption) {
            isEditingCaption = false;
        }
        else if (onBack) {
            onBack();
        }
        return;
    }

    backBtn.update();
    if (backBtn.isClicked()) {
        if (onBack) onBack();
        return;
    }

    Account* viewer = userManager ? userManager->findById(viewerUserId) : nullptr;
    bool isOwner = (viewerUserId != -1 && viewerUserId == post->getAuthorId());
    bool canDelete = isOwner || (viewer && viewer->canDeleteAnyPost());

    if (isEditingCaption) {
        editCaptionBox.update();
        saveEditBtn.update();
        cancelEditBtn.update();

        if (saveEditBtn.isClicked()) {
            std::string updated = editCaptionBox.getText();
            if (!updated.empty()) {
                post->setCaption(updated);
                if (postManager) postManager->save("data");
                toast.show("Caption updated", ToastType::Success);
            }
            isEditingCaption = false;
        }
        else if (cancelEditBtn.isClicked()) {
            isEditingCaption = false;
        }
    }

    commentBox.update();
    sendBtn.update();

    bool submitComment = sendBtn.isClicked() || (commentBox.isFocused() && IsKeyPressed(KEY_ENTER));
    if (submitComment && viewerUserId != -1) {
        std::string text = commentBox.getText();
        if (!text.empty()) {
            CommentNode node;
            node.id = nextCommentId++;
            node.parentId = replyingToCommentId;
            node.authorId = viewerUserId;
            node.text = text;
            node.timestamp = Time::now().toString();
            comments.push_back(node);

            commentBox.clear();
            replyingToCommentId = -1;
        }
    }

    Rectangle area = getContentArea();
    float pad = 16.0f;
    float innerW = area.width - pad * 2.0f;
    PostType type = post->getPostType();

    float bodyHeight = 64.0f;
    if (type == PostType::Photo || type == PostType::Carousel) {
        bodyHeight += 250.0f;
    }
    else if (type == PostType::Poll) {
        PollPost* poll = dynamic_cast<PollPost*>(post);
        std::size_t optCount = poll ? poll->getOptionCount() : 0;
        bodyHeight += 36.0f + (float)optCount * 42.0f;
    }
    else {
        bodyHeight += 74.0f;
    }

    bodyHeight += isEditingCaption ? 86.0f : 44.0f;
    bodyHeight += 46.0f;
    float commentsH = measureCommentTreeHeight(-1, 0, innerW);
    if (commentsH < 60.0f) commentsH = 60.0f;
    scrollView.setContentHeight(bodyHeight + commentsH + 24.0f);
    scrollView.update();

    Rectangle listArea = scrollView.getBounds();
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, listArea) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, authorHitRect) && onAuthorSelected) {
            onAuthorSelected(post->getAuthorId());
            return;
        }

        if (CheckCollisionPointRec(mouse, likeBtnRect) && viewerUserId != -1) {
            post->toggleLike(viewerUserId);
            if (postManager) postManager->save("data");
        }

        if (type == PostType::Carousel) {
            CarouselPost* carousel = dynamic_cast<CarouselPost*>(post);
            if (carousel && carousel->getImageCount() > 1) {
                if (CheckCollisionPointRec(mouse, carouselPrevRect)) {
                    carouselIndex = (carouselIndex == 0) ? (carousel->getImageCount() - 1) : (carouselIndex - 1);
                }
                else if (CheckCollisionPointRec(mouse, carouselNextRect)) {
                    carouselIndex = (carouselIndex + 1) % carousel->getImageCount();
                }
            }
        }

        if (type == PostType::Poll && viewerUserId != -1) {
            PollPost* poll = dynamic_cast<PollPost*>(post);
            if (poll) {
                for (std::size_t i = 0; i < pollOptionRects.size() && i < poll->getOptionCount(); i++) {
                    if (CheckCollisionPointRec(mouse, pollOptionRects[i])) {
                        poll->vote(viewerUserId, i);
                        if (postManager) postManager->save("data");
                        break;
                    }
                }
            }
        }

        if (isOwner && !isEditingCaption && CheckCollisionPointRec(mouse, editActionRect)) {
            isEditingCaption = true;
            editCaptionBox.setText(post->getCaption());
        }

        if (canDelete && viewer && CheckCollisionPointRec(mouse, deleteActionRect)) {
            int deletedId = post->getId();
            postManager->deletePost(deletedId, *viewer);
            pendingUndoPostId = deletedId;
            undoTimer = 5.0f;
            toast.show("Post deleted (5s to undo)", ToastType::Info, 5.0f);
            return;
        }

        if (!isOwner && viewerUserId != -1 && CheckCollisionPointRec(mouse, reportActionRect)) {
            reportModal.open("Report Post", "Report this post for violating campus guidelines?", "Report", "Cancel");
        }

        for (const CommentHitbox& hb : commentHitboxes) {
            if (CheckCollisionPointRec(mouse, hb.likeRect)) {
                toggleCommentLike(hb.commentId, viewerUserId);
                break;
            }
            if (CheckCollisionPointRec(mouse, hb.replyRect)) {
                replyingToCommentId = hb.commentId;
                break;
            }
        }
    }

    toast.update();
}

void PostDetailScreen::draw() const {
    Rectangle area = getContentArea();
    DrawRectangleRec(area, Theme::background());

    backBtn.draw();
    Rectangle backBounds = backBtn.getBounds();
    DrawTextEx(Theme::appFont, "Post", { backBounds.x + backBounds.width + 14.0f, backBounds.y + 6.0f }, 20.0f, 1.0f, Theme::primaryText());

    if (pendingUndoPostId != -1) {
        const char* pendingMsg = "Post moved to trash. Click Undo to restore.";
        Vector2 pSize = MeasureTextEx(Theme::appFont, pendingMsg, 15.0f, 1.0f);
        DrawTextEx(Theme::appFont, pendingMsg, { area.x + (area.width - pSize.x) / 2.0f, area.y + 140.0f }, 15.0f, 1.0f, Theme::secondaryText());

        DrawRectangleRounded(undoBtnRect, 0.25f, 8, Theme::accent());
        Vector2 uSize = MeasureTextEx(Theme::appFont, "Undo", 14.0f, 1.0f);
        DrawTextEx(Theme::appFont,
            "Undo",
            { undoBtnRect.x + (undoBtnRect.width - uSize.x) / 2.0f, undoBtnRect.y + (undoBtnRect.height - uSize.y) / 2.0f },
            14.0f,
            1.0f,
            Theme::isDark() ? Theme::background() : WHITE);

        toast.draw();
        return;
    }

    Post* post = postManager ? postManager->findById(postId) : nullptr;
    if (!post) return;

    Rectangle listArea = scrollView.getBounds();
    float offset = scrollView.getScrollOffset();
    float pad = 16.0f;
    float x = area.x + pad;
    float innerW = area.width - pad * 2.0f;
    float cursorY = listArea.y + 10.0f;

    Vector2 mouse = GetMousePosition();
    PostDetailScreen* self = const_cast<PostDetailScreen*>(this);
    self->pollOptionRects.clear();
    self->commentHitboxes.clear();

    scrollView.beginClip();

    Account* author = userManager ? userManager->findById(post->getAuthorId()) : nullptr;
    std::string authorName = author ? author->getDisplayName() : "Unknown";
    std::string authorHandle = author ? ("@" + author->getUsername()) : "@user";

    Rectangle headerRect = { x, cursorY - offset, innerW, 48.0f };
    self->authorHitRect = { x, cursorY - offset, innerW - 90.0f, 48.0f };

    Avatar avatar({ headerRect.x, headerRect.y + 4.0f, 40.0f, 40.0f }, makeInitials(authorName));
    avatar.draw();

    DrawTextEx(Theme::appFont, authorName.c_str(), { headerRect.x + 50.0f, headerRect.y + 4.0f }, 16.0f, 1.0f, Theme::primaryText());
    std::string subLine = authorHandle + "  •  " + post->getTimestamp().toString();
    DrawTextEx(Theme::appFont, subLine.c_str(), { headerRect.x + 50.0f, headerRect.y + 24.0f }, 12.5f, 1.0f, Theme::secondaryText());

    cursorY += 56.0f;

    PostType type = post->getPostType();
    if (type == PostType::Photo) {
        Rectangle mediaRect = { x, cursorY - offset, innerW, 240.0f };
        DrawRectangleRounded(mediaRect, 0.06f, 8, Theme::surface());
        PhotoPost* photo = dynamic_cast<PhotoPost*>(post);
        bool drew = false;
        if (photo && !photo->getImagePath().empty()) {
            Texture2D tex = ImageStore::getTexture(photo->getImagePath());
            if (tex.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
                DrawTexturePro(tex, src, mediaRect, { 0, 0 }, 0.0f, WHITE);
                drew = true;
            }
        }
        if (!drew) {
            const char* lbl = "[Photo]";
            Vector2 lSize = MeasureTextEx(Theme::appFont, lbl, 16.0f, 1.0f);
            DrawTextEx(Theme::appFont, lbl, { mediaRect.x + (mediaRect.width - lSize.x) / 2.0f, mediaRect.y + 110.0f }, 16.0f, 1.0f, Theme::secondaryText());
        }
        cursorY += 248.0f;
    }
    else if (type == PostType::Carousel) {
        Rectangle mediaRect = { x, cursorY - offset, innerW, 240.0f };
        DrawRectangleRounded(mediaRect, 0.06f, 8, Theme::surface());
        CarouselPost* carousel = dynamic_cast<CarouselPost*>(post);
        std::size_t imgCount = carousel ? carousel->getImageCount() : 0;
        std::size_t activeIdx = (imgCount > 0) ? (carouselIndex % imgCount) : 0;

        bool drew = false;
        if (carousel && imgCount > 0) {
            const std::string& path = (*carousel)[activeIdx];
            Texture2D tex = ImageStore::getTexture(path);
            if (tex.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
                DrawTexturePro(tex, src, mediaRect, { 0, 0 }, 0.0f, WHITE);
                drew = true;
            }
        }
        if (!drew) {
            std::string lbl = "Slide " + std::to_string(activeIdx + 1) + " / " + std::to_string(std::max<std::size_t>(1, imgCount));
            Vector2 lSize = MeasureTextEx(Theme::appFont, lbl.c_str(), 16.0f, 1.0f);
            DrawTextEx(Theme::appFont, lbl.c_str(), { mediaRect.x + (mediaRect.width - lSize.x) / 2.0f, mediaRect.y + 108.0f }, 16.0f, 1.0f, Theme::secondaryText());
        }

        if (imgCount > 1) {
            self->carouselPrevRect = { mediaRect.x + 10.0f, mediaRect.y + 104.0f, 32.0f, 32.0f };
            self->carouselNextRect = { mediaRect.x + mediaRect.width - 42.0f, mediaRect.y + 104.0f, 32.0f, 32.0f };

            DrawRectangleRounded(carouselPrevRect, 0.5f, 8, Fade(BLACK, 0.55f));
            DrawRectangleRounded(carouselNextRect, 0.5f, 8, Fade(BLACK, 0.55f));
            DrawTextEx(Theme::appFont, "<", { carouselPrevRect.x + 11.0f, carouselPrevRect.y + 7.0f }, 16.0f, 1.0f, WHITE);
            DrawTextEx(Theme::appFont, ">", { carouselNextRect.x + 11.0f, carouselNextRect.y + 7.0f }, 16.0f, 1.0f, WHITE);

            float dotsW = imgCount * 12.0f;
            float dotStartX = mediaRect.x + (mediaRect.width - dotsW) / 2.0f;
            for (std::size_t d = 0; d < imgCount; d++) {
                Color dotCol = (d == activeIdx) ? Theme::accent() : Fade(WHITE, 0.6f);
                DrawCircle((int)(dotStartX + d * 12.0f + 4.0f), (int)(mediaRect.y + mediaRect.height - 14.0f), 4.0f, dotCol);
            }
        }
        cursorY += 248.0f;
    }
    else if (type == PostType::Poll) {
        PollPost* poll = dynamic_cast<PollPost*>(post);
        if (poll) {
            DrawTextEx(Theme::appFont, poll->getQuestion().c_str(), { x, cursorY - offset }, 16.0f, 1.0f, Theme::primaryText());
            cursorY += 28.0f;

            int totalVotes = poll->getTotalVotes();
            bool voted = (viewerUserId != -1) && poll->hasVoted(viewerUserId);

            for (std::size_t i = 0; i < poll->getOptionCount(); i++) {
                Rectangle optRect = { x, cursorY - offset, innerW, 34.0f };
                self->pollOptionRects.push_back(optRect);

                DrawRectangleRounded(optRect, 0.2f, 8, Theme::surface());
                DrawRectangleRoundedLinesEx(optRect, 0.2f, 8, 1.0f, Theme::border());

                int optVotes = poll->getVoteCount(i);
                float pct = (totalVotes > 0) ? ((float)optVotes / (float)totalVotes) : 0.0f;
                if (voted && pct > 0.0f) {
                    Rectangle fillRect = { optRect.x, optRect.y, optRect.width * pct, optRect.height };
                    DrawRectangleRounded(fillRect, 0.2f, 8, Fade(Theme::accent(), 0.28f));
                }

                DrawTextEx(Theme::appFont, poll->getOption(i).c_str(), { optRect.x + 12.0f, optRect.y + 8.0f }, 14.0f, 1.0f, Theme::primaryText());

                if (voted) {
                    std::string pctStr = std::to_string((int)(pct * 100.0f + 0.5f)) + "%";
                    Vector2 pSize = MeasureTextEx(Theme::appFont, pctStr.c_str(), 13.0f, 1.0f);
                    DrawTextEx(Theme::appFont, pctStr.c_str(), { optRect.x + optRect.width - pSize.x - 12.0f, optRect.y + 9.0f }, 13.0f, 1.0f, Theme::secondaryText());
                }

                cursorY += 40.0f;
            }
        }
    }
    else {
        Rectangle textCard = { x, cursorY - offset, innerW, 66.0f };
        DrawRectangleRounded(textCard, 0.14f, 8, Theme::surface());
        DrawRectangleRoundedLinesEx(textCard, 0.14f, 8, 1.0f, Theme::border());
        std::string body = post->getCaption().empty() ? "(Empty post)" : post->getCaption();
        if (body.size() > 58) body = body.substr(0, 55) + "...";
        DrawTextEx(Theme::appFont, body.c_str(), { textCard.x + 12.0f, textCard.y + 22.0f }, 15.0f, 1.0f, Theme::primaryText());
        cursorY += 74.0f;
    }

    if (isEditingCaption) {
        self->editCaptionBox.setBounds({ x, cursorY - offset, innerW, 36.0f });
        self->saveEditBtn.setBounds({ x, cursorY + 42.0f - offset, 76.0f, 30.0f });
        self->cancelEditBtn.setBounds({ x + 84.0f, cursorY + 42.0f - offset, 76.0f, 30.0f });
        editCaptionBox.draw();
        saveEditBtn.draw();
        cancelEditBtn.draw();
        cursorY += 80.0f;
    }
    else if (type == PostType::Photo || type == PostType::Carousel || type == PostType::Poll) {
        if (!post->getCaption().empty()) {
            DrawTextEx(Theme::appFont, post->getCaption().c_str(), { x, cursorY - offset }, 15.0f, 1.0f, Theme::primaryText());
            cursorY += 28.0f;
        }
    }

    bool postLiked = (viewerUserId != -1) && post->isLikedBy(viewerUserId);
    std::string likeText = (postLiked ? "Liked  " : "Like  ") + std::to_string(post->getLikeCount());
    self->likeBtnRect = { x, cursorY - offset, 92.0f, 30.0f };
    DrawRectangleRounded(likeBtnRect, 0.25f, 8, postLiked ? Fade(Theme::danger(), 0.18f) : Theme::surface());
    DrawRectangleRoundedLinesEx(likeBtnRect, 0.25f, 8, 1.0f, postLiked ? Theme::danger() : Theme::border());
    DrawTextEx(Theme::appFont, likeText.c_str(), { likeBtnRect.x + 12.0f, likeBtnRect.y + 7.0f }, 13.5f, 1.0f, postLiked ? Theme::danger() : Theme::primaryText());

    Account* viewer = userManager ? userManager->findById(viewerUserId) : nullptr;
    bool isOwner = (viewerUserId != -1 && viewerUserId == post->getAuthorId());
    bool canDelete = isOwner || (viewer && viewer->canDeleteAnyPost());

    float actionRightX = x + innerW;
    if (!isOwner && viewerUserId != -1) {
        self->reportActionRect = { actionRightX - 64.0f, cursorY - offset, 64.0f, 30.0f };
        DrawTextEx(Theme::appFont, "Report", { reportActionRect.x + 10.0f, reportActionRect.y + 7.0f }, 13.5f, 1.0f, Theme::secondaryText());
        actionRightX -= 68.0f;
    }
    if (canDelete) {
        self->deleteActionRect = { actionRightX - 64.0f, cursorY - offset, 64.0f, 30.0f };
        DrawTextEx(Theme::appFont, "Delete", { deleteActionRect.x + 10.0f, deleteActionRect.y + 7.0f }, 13.5f, 1.0f, Theme::danger());
        actionRightX -= 68.0f;
    }
    if (isOwner && !isEditingCaption) {
        self->editActionRect = { actionRightX - 50.0f, cursorY - offset, 50.0f, 30.0f };
        DrawTextEx(Theme::appFont, "Edit", { editActionRect.x + 10.0f, editActionRect.y + 7.0f }, 13.5f, 1.0f, Theme::accent());
    }

    cursorY += 40.0f;
    DrawLineEx({ x, cursorY - offset }, { x + innerW, cursorY - offset }, 1.0f, Theme::border());
    cursorY += 10.0f;

    std::string commentHeader = "Comments (" + std::to_string(comments.size()) + ")";
    DrawTextEx(Theme::appFont, commentHeader.c_str(), { x, cursorY - offset }, 15.0f, 1.0f, Theme::primaryText());
    cursorY += 26.0f;

    if (comments.empty()) {
        DrawTextEx(Theme::appFont, "No comments yet. Start the conversation!", { x, cursorY - offset + 8.0f }, 13.5f, 1.0f, Theme::secondaryText());
    }
    else {
        drawCommentTree(-1, 0, x, cursorY, innerW, offset, listArea, mouse);
    }

    scrollView.endClip();
    scrollView.draw();

    Rectangle boxBounds = commentBox.getBounds();
    DrawLineEx({ area.x, boxBounds.y - 8.0f - (replyingToCommentId != -1 ? 22.0f : 0.0f) },
        { area.x + area.width, boxBounds.y - 8.0f - (replyingToCommentId != -1 ? 22.0f : 0.0f) },
        1.0f,
        Theme::border());

    if (replyingToCommentId != -1) {
        std::string replyHint = "Replying to comment #" + std::to_string(replyingToCommentId) + "  (Esc to cancel)";
        DrawTextEx(Theme::appFont, replyHint.c_str(), { x, boxBounds.y - 22.0f }, 12.0f, 1.0f, Theme::accent());
    }

    commentBox.draw();
    sendBtn.draw();

    toast.draw();
    reportModal.draw();
}