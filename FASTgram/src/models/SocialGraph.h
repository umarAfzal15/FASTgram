#pragma once
#include <vector>
#include <utility>

class SocialGraph {
private:
    std::vector<std::pair<int, int>> followEdges;
    std::vector<std::pair<int, int>> pendingRequests;
    std::vector<std::pair<int, int>> blockedEdges;
    std::vector<std::pair<int, int>> mutedEdges;

public:
    void follow(int followerId, int targetId, bool isTargetPrivate = false) {
        if (followerId == targetId) return;
        if (isTargetPrivate) {
            for (int i = 0; i < (int)pendingRequests.size(); i++) {
                if (pendingRequests[i].first == followerId && pendingRequests[i].second == targetId) return;
            }
            if (!isFollowing(followerId, targetId)) pendingRequests.push_back({ followerId, targetId });
            return;
        }
        if (!isFollowing(followerId, targetId)) followEdges.push_back({ followerId, targetId });
    }

    void unfollow(int followerId, int targetId) {
        for (int i = 0; i < (int)followEdges.size(); i++) {
            if (followEdges[i].first == followerId && followEdges[i].second == targetId) {
                followEdges.erase(followEdges.begin() + i);
                return;
            }
        }
    }

    bool isFollowing(int followerId, int targetId) const {
        for (int i = 0; i < (int)followEdges.size(); i++) {
            if (followEdges[i].first == followerId && followEdges[i].second == targetId) return true;
        }
        return false;
    }

    std::vector<int> getFollowers(int userId) const {
        std::vector<int> followers;
        for (int i = 0; i < (int)followEdges.size(); i++) {
            if (followEdges[i].second == userId) followers.push_back(followEdges[i].first);
        }
        return followers;
    }

    std::vector<int> getFollowing(int userId) const {
        std::vector<int> following;
        for (int i = 0; i < (int)followEdges.size(); i++) {
            if (followEdges[i].first == userId) following.push_back(followEdges[i].second);
        }
        return following;
    }

    int getFollowerCount(int userId) const { return (int)getFollowers(userId).size(); }
    int getFollowingCount(int userId) const { return (int)getFollowing(userId).size(); }

    // Feature R6: Follow Request State Machine
    bool hasPendingRequest(int followerId, int targetId) const {
        for (int i = 0; i < (int)pendingRequests.size(); i++) {
            if (pendingRequests[i].first == followerId && pendingRequests[i].second == targetId) return true;
        }
        return false;
    }

    void acceptFollowRequest(int followerId, int targetId) {
        for (int i = 0; i < (int)pendingRequests.size(); i++) {
            if (pendingRequests[i].first == followerId && pendingRequests[i].second == targetId) {
                pendingRequests.erase(pendingRequests.begin() + i);
                if (!isFollowing(followerId, targetId)) {
                    followEdges.push_back({ followerId, targetId });
                }
                return;
            }
        }
    }

    void declineFollowRequest(int followerId, int targetId) {
        for (int i = 0; i < (int)pendingRequests.size(); i++) {
            if (pendingRequests[i].first == followerId && pendingRequests[i].second == targetId) {
                pendingRequests.erase(pendingRequests.begin() + i);
                return;
            }
        }
    }

    // Feature R4: Block & Mute
    void blockUser(int blockerId, int targetId) {
        if (blockerId == targetId) return;
        unfollow(blockerId, targetId);
        unfollow(targetId, blockerId);
        declineFollowRequest(blockerId, targetId);
        declineFollowRequest(targetId, blockerId);
        if (!isBlocked(blockerId, targetId)) {
            blockedEdges.push_back({ blockerId, targetId });
        }
    }

    void unblockUser(int blockerId, int targetId) {
        for (int i = 0; i < (int)blockedEdges.size(); i++) {
            if (blockedEdges[i].first == blockerId && blockedEdges[i].second == targetId) {
                blockedEdges.erase(blockedEdges.begin() + i);
                return;
            }
        }
    }

    bool isBlocked(int blockerId, int targetId) const {
        for (int i = 0; i < (int)blockedEdges.size(); i++) {
            if (blockedEdges[i].first == blockerId && blockedEdges[i].second == targetId) return true;
        }
        return false;
    }

    void muteUser(int muterId, int targetId) {
        if (muterId == targetId) return;
        if (!isMuted(muterId, targetId)) {
            mutedEdges.push_back({ muterId, targetId });
        }
    }

    void unmuteUser(int muterId, int targetId) {
        for (int i = 0; i < (int)mutedEdges.size(); i++) {
            if (mutedEdges[i].first == muterId && mutedEdges[i].second == targetId) {
                mutedEdges.erase(mutedEdges.begin() + i);
                return;
            }
        }
    }

    bool isMuted(int muterId, int targetId) const {
        for (int i = 0; i < (int)mutedEdges.size(); i++) {
            if (mutedEdges[i].first == muterId && mutedEdges[i].second == targetId) return true;
        }
        return false;
    }

    // Feature A5 & Shared Contract #3: Centralized Visibility Rule
    bool canViewContent(int viewerId, int ownerId, bool isOwnerPrivate) const {
        if (viewerId == ownerId) return true;
        if (isBlocked(ownerId, viewerId) || isBlocked(viewerId, ownerId)) return false;
        if (!isOwnerPrivate) return true;
        return isFollowing(viewerId, ownerId);
    }

    // Day 4: Binary serialization helpers for follows.dat
    const std::vector<std::pair<int, int>>& getFollowEdges() const { return followEdges; }
    const std::vector<std::pair<int, int>>& getPendingRequestEdges() const { return pendingRequests; }
    const std::vector<std::pair<int, int>>& getBlockedEdges() const { return blockedEdges; }
    const std::vector<std::pair<int, int>>& getMutedEdges() const { return mutedEdges; }

    void addLoadedFollow(int followerId, int targetId) {
        if (followerId != targetId && !isFollowing(followerId, targetId)) {
            followEdges.push_back({ followerId, targetId });
        }
    }
    void addLoadedPendingRequest(int followerId, int targetId) {
        if (followerId != targetId && !hasPendingRequest(followerId, targetId)) {
            pendingRequests.push_back({ followerId, targetId });
        }
    }
    void addLoadedBlock(int blockerId, int targetId) {
        if (blockerId != targetId && !isBlocked(blockerId, targetId)) {
            blockedEdges.push_back({ blockerId, targetId });
        }
    }
    void addLoadedMute(int muterId, int targetId) {
        if (muterId != targetId && !isMuted(muterId, targetId)) {
            mutedEdges.push_back({ muterId, targetId });
        }
    }

    // Day 4: Cascade delete all edges involving userId
    void removeUser(int userId) {
        for (int i = (int)followEdges.size() - 1; i >= 0; i--) {
            if (followEdges[i].first == userId || followEdges[i].second == userId) {
                followEdges.erase(followEdges.begin() + i);
            }
        }
        for (int i = (int)pendingRequests.size() - 1; i >= 0; i--) {
            if (pendingRequests[i].first == userId || pendingRequests[i].second == userId) {
                pendingRequests.erase(pendingRequests.begin() + i);
            }
        }
        for (int i = (int)blockedEdges.size() - 1; i >= 0; i--) {
            if (blockedEdges[i].first == userId || blockedEdges[i].second == userId) {
                blockedEdges.erase(blockedEdges.begin() + i);
            }
        }
        for (int i = (int)mutedEdges.size() - 1; i >= 0; i--) {
            if (mutedEdges[i].first == userId || mutedEdges[i].second == userId) {
                mutedEdges.erase(mutedEdges.begin() + i);
            }
        }
    }
};