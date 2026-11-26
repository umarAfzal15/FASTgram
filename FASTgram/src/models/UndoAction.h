#pragma once
#pragma once

class Post;

class UndoDeleteAction {
private:
    Post* deletedPost;
    int originalIndex;
    float timeRemaining;
    bool active;

public:
    UndoDeleteAction();
    UndoDeleteAction(Post* post, int index, float duration = 5.0f);

    void tick(float dt);
    bool canUndo() const;
    bool isExpired() const;

    Post* undo();
    void commitDelete();

    int getOriginalIndex() const;
    float getTimeRemaining() const;
};
