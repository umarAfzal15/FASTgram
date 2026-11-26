#include "NewestFirstSorter.h"
#include "../src/models/Post.h"

std::string NewestFirstSorter::getName() const {
	return "Newest";
}

bool NewestFirstSorter::operator()(const Post* a, const Post* b) const {
    if (a == nullptr || b == nullptr) {
        return false;
    }

    if (*b < *a) {
        return true;
    }
    else {
        return false;
    }
}