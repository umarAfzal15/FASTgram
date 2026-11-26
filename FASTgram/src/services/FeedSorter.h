#pragma once

#include <string>

// Forward declarations prevent Core from directly depending on Models/Services headers and #include loop
class Post;
class SocialGraph;

class FeedSorter {
public:
    virtual ~FeedSorter() = default;

    // Label for the UI sort chip/button on HomeScreen
    virtual std::string getName() const = 0;

    // Optional visibility/membership check (used by FollowingOnlyFilter; defaults to true)
    virtual bool shouldInclude(const Post* post) const {
        if (post != nullptr) {
            return true;
        }
        else {
            return false;
        }
    }

    //returns true if post 'a' should appear before post 'b'
    virtual bool operator()(const Post* a, const Post* b) const = 0;
};