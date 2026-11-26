#include "Post.h"

int Post::nextId = 1;

Post::Post()
    : id(0), authorId(0), caption(""), timestamp(), isArchived(false), isPinned(false) {
}

Post::Post(int authorId, const std::string& caption, const Time& timestamp)
    : id(nextId++), authorId(authorId), caption(caption), timestamp(timestamp),
    isArchived(false), isPinned(false) {
}

Post::Post(int id, int authorId, const std::string& caption, const Time& timestamp)
    : id(id), authorId(authorId), caption(caption), timestamp(timestamp),
    isArchived(false), isPinned(false) {
    if (id >= nextId) {
        nextId = id + 1;
    }
}

int Post::getId() const {
    return id;
}

int Post::getAuthorId() const {
    return authorId;
}

std::string Post::getCaption() const {
    return caption;
}

const Time& Post::getTimestamp() const {
    return timestamp;
}

bool Post::getIsArchived() const {
    return isArchived;
}

bool Post::getIsPinned() const {
    return isPinned;
}

void Post::setCaption(const std::string& newCaption) {
    caption = newCaption;
}

void Post::setArchived(bool archived) {
    isArchived = archived;
}

void Post::setPinned(bool pinned) {
    isPinned = pinned;
}

void Post::toggleLike(int userId) {
    for (int i = 0; i < static_cast<int>(likedUserIds.size()); i++) {
        if (likedUserIds[i] == userId) {
            likedUserIds.erase(likedUserIds.begin() + i);
            return;
        }
    }
    likedUserIds.push_back(userId);
}

int Post::getLikeCount() const {
    return static_cast<int>(likedUserIds.size());
}

bool Post::isLikedBy(int userId) const {
    for (int i = 0; i < static_cast<int>(likedUserIds.size()); i++) {
        if (likedUserIds[i] == userId) {
            return true;
        }
    }
    return false;
}

bool Post::operator<(const Post& other) const {
    return id < other.id;
}

void Post::write(std::ostream& out) const {
    out.write(reinterpret_cast<const char*>(&id), sizeof(id));
    out.write(reinterpret_cast<const char*>(&authorId), sizeof(authorId));

    int captionLen = static_cast<int>(caption.size());
    out.write(reinterpret_cast<const char*>(&captionLen), sizeof(captionLen));
    if (captionLen > 0) {
        out.write(caption.data(), captionLen);
    }

    int likeCount = static_cast<int>(likedUserIds.size());
    out.write(reinterpret_cast<const char*>(&likeCount), sizeof(likeCount));
    for (int i = 0; i < likeCount; i++) {
        int uid = likedUserIds[i];
        out.write(reinterpret_cast<const char*>(&uid), sizeof(uid));
    }

    out.write(reinterpret_cast<const char*>(&isArchived), sizeof(isArchived));
    out.write(reinterpret_cast<const char*>(&isPinned), sizeof(isPinned));
}

void Post::read(std::istream& in) {
    in.read(reinterpret_cast<char*>(&id), sizeof(id));
    in.read(reinterpret_cast<char*>(&authorId), sizeof(authorId));

    int captionLen = 0;
    in.read(reinterpret_cast<char*>(&captionLen), sizeof(captionLen));
    if (captionLen > 0) {
        caption.resize(captionLen);
        in.read(&caption[0], captionLen);
    }
    else {
        caption = "";
    }

    int likeCount = 0;
    in.read(reinterpret_cast<char*>(&likeCount), sizeof(likeCount));
    likedUserIds.clear();
    for (int i = 0; i < likeCount; i++) {
        int uid = 0;
        in.read(reinterpret_cast<char*>(&uid), sizeof(uid));
        likedUserIds.push_back(uid);
    }

    in.read(reinterpret_cast<char*>(&isArchived), sizeof(isArchived));
    in.read(reinterpret_cast<char*>(&isPinned), sizeof(isPinned));

    if (id >= nextId) {
        nextId = id + 1;
    }
}

int Post::getNextId() {
    return nextId;
}

void Post::setNextId(int value) {
    nextId = value;
}