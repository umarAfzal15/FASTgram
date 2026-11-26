#pragma once
#include <string>
#include <vector>
#include "../models/Account.h"
#include "../models/Post.h"

class TextFileManager {
public:
    // Feature P6 & D2: Rebuild data/tags.txt (#hashtag -> post IDs)s
    static void rebuildTagsIndex(const std::vector<Post*>& posts);
    static std::vector<int> getPostIdsByTag(const std::string& tag);

    // Feature D2: Append a report line to data/reports.txt
    static bool appendReport(int postId, int reporterId, const std::string& reason);

    // Feature D5: Export human-readable profile summary to data/export_<username>.txt
    static bool exportUserSummary(const Account* acc, int postCount, int followers, int following);
};