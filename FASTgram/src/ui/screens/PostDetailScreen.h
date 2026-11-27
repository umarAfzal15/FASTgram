#pragma once
#include "Screen.h"
#include "../Widgets/ScrollView.h"
#include "../Widgets/Button.h"
#include "../Widgets/TextBox.h"
#include "../Widgets/Avatar.h"
#include "../Widgets/Toast.h"
#include "../Widgets/Modal.h"
#include "../../services/PostManager.h"
#include "../../services/UserManager.h"
#include "../../models/TextFileManager.h"
#include "../../services/ImageStore.h"
#include "../../models/Post.h"
#include "../../models/PhotoPost.h"
#include "../../models/CarouselPost.h"
#include "../../models/PollPost.h"
#include "../../models/Comment.h"
#include "../../models/Account.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

class PostDetailScreen : public Screen {
private:
    struct CommentNode {
        int id;
        int parentId;
        int authorId;
        std::string text;
        std::string timestamp;
        std::vector<int> likedBy;
    };

    struct CommentHitbox {
        int commentId;
        Rectangle likeRect;
        Rectangle replyRect;
    };

    int postId;
    int viewerUserId;

    PostManager* postManager;
    UserManager* userManager;

    std::function<void()> onBack;
    std::function<void(int)> onAuthorSelected;

    Button backBtn;
    TextBox commentBox;
    Button sendBtn;
    ScrollView scrollView;
    Toast toast;
    Modal reportModal;

    std::size_t carouselIndex;
    bool isEditingCaption;
    TextBox editCaptionBox;
    Button saveEditBtn;
    Button cancelEditBtn;

    int pendingUndoPostId;
    float undoTimer;

    int replyingToCommentId;
    int nextCommentId;
    std::vector<CommentNode> comments;
    mutable std::vector<CommentHitbox> commentHitboxes;

    Rectangle authorHitRect;
    Rectangle likeBtnRect;
    Rectangle carouselPrevRect;
    Rectangle carouselNextRect;
    Rectangle editActionRect;
    Rectangle deleteActionRect;
    Rectangle reportActionRect;
    Rectangle undoBtnRect;
    std::vector<Rectangle> pollOptionRects;

    static constexpr float COLUMN_WIDTH = 480.0f;

    Rectangle getContentArea() const;
    std::string makeInitials(const std::string& name) const;
    void updateLayout();
    void toggleCommentLike(int commentId, int userId);
    int countReplies(int parentId) const;
    float measureCommentTreeHeight(int parentId, int depth, float maxW) const;
    void drawCommentTree(int parentId, int depth, float startX, float& cursorY, float maxW, float offset, Rectangle listArea, Vector2 mouse) const;

public:
    PostDetailScreen(int targetPostId,
        PostManager* posts = nullptr,
        UserManager* users = nullptr,
        int currentViewerId = -1,
        std::function<void()> onBackCallback = nullptr,
        std::function<void(int)> onAuthorCallback = nullptr);

    void setViewerId(int currentViewerId);
    void setOnBack(std::function<void()> callback);
    void setOnAuthorSelected(std::function<void(int)> callback);

    void onEnter() override;
    void update() override;
    void draw() const override;
};