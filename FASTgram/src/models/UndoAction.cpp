#include "UndoAction.h"
#include "../models/Post.h"

UndoDeleteAction::UndoDeleteAction()
    : deletedPost(nullptr), originalIndex(-1), timeRemaining(0.0f), active(false) {
}

UndoDeleteAction::UndoDeleteAction(Post* post, int index, float duration)
    : deletedPost(post), originalIndex(index), timeRemaining(duration), active(post != nullptr) {
}

void UndoDeleteAction::tick(float dt) {
    if (!active) return;

    timeRemaining -= dt;
    if (timeRemaining <= 0.0f) {
        timeRemaining = 0.0f;
        commitDelete();
    }
}

bool UndoDeleteAction::canUndo() const {
    return active && deletedPost != nullptr && timeRemaining > 0.0f;
}

bool UndoDeleteAction::isExpired() const {
    return active && timeRemaining <= 0.0f;
}

Post* UndoDeleteAction::undo() {
    if (!canUndo()) return nullptr;

    Post* restored = deletedPost;
    deletedPost = nullptr;
    active = false;
    timeRemaining = 0.0f;
    return restored;
}

void UndoDeleteAction::commitDelete() {
    if (deletedPost != nullptr) {
        delete deletedPost;
        deletedPost = nullptr;
    }
    active = false;
    timeRemaining = 0.0f;
}

int UndoDeleteAction::getOriginalIndex() const {
    return originalIndex;
}

float UndoDeleteAction::getTimeRemaining() const {
    return timeRemaining;
}