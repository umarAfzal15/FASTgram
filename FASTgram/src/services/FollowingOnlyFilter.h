#pragma once

#include "FeedSorter.h"

class FollowingOnlyFilter : public FeedSorter {
private:
    const SocialGraph* socialGraph;
    int currentUserId;

public:
    FollowingOnlyFilter(const SocialGraph* graph, int userId);

    std::string getName() const override;
    bool shouldInclude(const Post* post) const override;
    bool operator()(const Post* a, const Post* b) const override;
};