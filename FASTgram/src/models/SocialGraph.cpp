#include "SocialGraph.h"

void SocialGraph::follow(int followerId, int targetId, bool isTargetPrivate) {
    if (followerId == targetId)
        return;

    if (isTargetPrivate) {
        for (int i = 0; i < (int)pendingRequests.size(); i++) {
            if (pendingRequests[i].first == followerId && pendingRequests[i].second == targetId)
                return;
        }
        if (!isFollowing(followerId, targetId)) {
            pendingRequests.push_back({ followerId, targetId });
        }
        return;
    }

    if (!isFollowing(followerId, targetId)) {
        followEdges.push_back({ followerId, targetId });
    }
}

void SocialGraph::unfollow(int followerId, int targetId) {
    for (int i = 0; i < (int)followEdges.size(); i++) {
        if (followEdges[i].first == followerId && followEdges[i].second == targetId) {
            followEdges.erase(followEdges.begin() + i);
            return;
        }
    }
}

bool SocialGraph::isFollowing(int followerId, int targetId) const {
    for (int i = 0; i < (int)followEdges.size(); i++) {
        if (followEdges[i].first == followerId && followEdges[i].second == targetId) {
            return true;
        }
    }
    return false;
}

std::vector<int> SocialGraph::getFollowers(int userId) const {
    std::vector<int> followers;
    for (int i = 0; i < (int)followEdges.size(); i++) {
        if (followEdges[i].second == userId) {
            followers.push_back(followEdges[i].first);
        }
    }
    return followers;
}

std::vector<int> SocialGraph::getFollowing(int userId) const {
    std::vector<int> following;
    for (int i = 0; i < (int)followEdges.size(); i++) {
        if (followEdges[i].first == userId) {
            following.push_back(followEdges[i].second);
        }
    }
    return following;
}

int SocialGraph::getFollowerCount(int userId) const {
    return (int)getFollowers(userId).size();
}

int SocialGraph::getFollowingCount(int userId) const {
    return (int)getFollowing(userId).size();
}