#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../core/Serializable.h"
#include "../core/Time.h"

enum class PostType {
    Photo = 0,
    Text = 1,
    Carousel = 2,
    Poll = 3,
    Notice = 4,
    LostFound = 5
};

class Post : public Serializable {
private:
    int id;
    int authorId;
    std::string caption;
    Time timestamp;
    std::vector<int> likedUserIds;
    bool archived;
    bool pinned;

    static int nextId;

protected:
    Post();

public:
    Post(int authorId, const std::string& caption);
    Post(int authorId, const std::string& caption, const Time& timestamp);
    Post(int id, int authorId, const std::string& caption, const Time& timestamp);
    virtual ~Post() = default;

    // Shared Code Contract getters
    int getId() const;
    int getAuthorId() const;
    std::string getCaption() const;
    const Time& getTimestamp() const;
    bool getIsArchived() const;
    bool getIsPinned() const;
    bool isArchived() const;
    bool isPinned() const;

    virtual void setCaption(const std::string& newCaption);
    void setArchived(bool archived);
    void setPinned(bool pinned);

    // Feature I1: Like/unlike encapsulation
    void toggleLike(int userId);
    int getLikeCount() const;
    bool isLikedBy(int userId) const;

    // Feature F1: Sorting comparison (older < newer)
    bool operator<(const Post& other) const;

    // Polymorphic interface
    virtual PostType getPostType() const = 0;

    // Serializable overrides
    void write(std::ostream& out) const override;
    void read(std::istream& in) override;

    static int getNextId();
    static void setNextId(int value);
};