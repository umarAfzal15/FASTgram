#include "FollowingOnlyFilter.h"
#include "../src/models/SocialGraph.h"
#include "../models/Post.h"

FollowingOnlyFilter::FollowingOnlyFilter(const SocialGraph* graph, int userId) {
    socialGraph = graph;
    currentUserId = userId;
}

std::string FollowingOnlyFilter::getName() const {
    return "Following";
}

bool FollowingOnlyFilter::shouldInclude(const Post* post) const {
    if (post == nullptr || socialGraph == nullptr) {
        return false;
    }

    int authorId = post->getAuthorId();

    if (socialGraph->isFollowing(currentUserId, authorId)) {
        return true;
    }
    else {
        return false;
    }
}

bool FollowingOnlyFilter::operator()(const Post* a, const Post* b) const {
    if (a == nullptr || b == nullptr) {
        return false;
    }

    // Order the filtered "Following" posts newest first
    if (*b < *a) {
        return true;
    }
    else {
        return false;
    }
}