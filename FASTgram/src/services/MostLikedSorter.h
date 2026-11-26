#pragma once

#include "FeedSorter.h"

class MostLikedSorter : public FeedSorter {
public:
    std::string getName() const override;
    bool operator()(const Post* a, const Post* b) const override;
};