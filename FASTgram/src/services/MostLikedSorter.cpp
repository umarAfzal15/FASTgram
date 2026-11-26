#include "MostLikedSorter.h"
#include "../models/Post.h"

std::string MostLikedSorter::getName() const {
    return "Most Liked";
}

bool MostLikedSorter::operator()(const Post* a, const Post* b) const {
    if (a == nullptr || b == nullptr) {
        return false;
    }

    if (a->getLikeCount() > b->getLikeCount()) {
        return true;
    }
    else if (a->getLikeCount() < b->getLikeCount()) {
        return false;
    }
    else {
        //if both posts have equal likes, put the newer post first
        if (*b < *a) {
            return true;
        }
        else {
            return false;
        }
    }
}