#include "TextFileManager.h"
#include "TextParser.h"
#include <fstream>
#include <sstream>

void TextFileManager::rebuildTagsIndex(const std::vector<Post*>& posts) {
    std::vector<std::string> uniqueTags;
    std::vector<std::vector<int>> tagPostIds;

    for (int i = 0; i < (int)posts.size(); i++) {
        if (posts[i] == nullptr) continue;

        std::vector<std::string> tags = TextParser::extractHashtags(posts[i]->getCaption());
        for (int j = 0; j < (int)tags.size(); j++) {
            int foundIdx = -1;
            for (int k = 0; k < (int)uniqueTags.size(); k++) {
                if (uniqueTags[k] == tags[j]) {
                    foundIdx = k;
                    break;
                }
            }

            if (foundIdx == -1) {
                uniqueTags.push_back(tags[j]);
                std::vector<int> ids;
                ids.push_back(posts[i]->getId());
                tagPostIds.push_back(ids);
            }
            else {
                tagPostIds[foundIdx].push_back(posts[i]->getId());
            }
        }
    }

    std::ofstream out("data/tags.txt");
    if (!out.is_open()) return;

    for (int i = 0; i < (int)uniqueTags.size(); i++) {
        out << uniqueTags[i];
        for (int j = 0; j < (int)tagPostIds[i].size(); j++) {
            out << " " << tagPostIds[i][j];
        }
        out << "\n";
    }
    out.close();
}

std::vector<int> TextFileManager::getPostIdsByTag(const std::string& tag) {
    std::vector<int> result;
    std::ifstream in("data/tags.txt");
    if (!in.is_open()) return result;

    std::string target = tag;
    if (!target.empty() && target[0] != '#') {
        target = "#" + target;
    }

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string fileTag;
        if (iss >> fileTag && fileTag == target) {
            int id;
            while (iss >> id) {
                result.push_back(id);
            }
            break;
        }
    }
    in.close();
    return result;
}

bool TextFileManager::appendReport(int postId, int reporterId, const std::string& reason) {
    std::ofstream out("data/reports.txt", std::ios::app);
    if (!out.is_open()) return false;

    out << "PostID: " << postId
        << " | ReporterID: " << reporterId
        << " | Reason: " << reason << "\n";
    out.close();
    return true;
}

bool TextFileManager::exportUserSummary(const Account* acc, int postCount, int followers, int following) {
    if (acc == nullptr) return false;

    std::string filename = "data/export_" + acc->getUsername() + ".txt";
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    out << "=== FASTagram Profile Export ===\n";
    out << "Account ID   : " << acc->getId() << "\n";
    out << "Username     : @" << acc->getUsername() << "\n";
    out << "Display Name : " << acc->getDisplayName() << "\n";
    out << "Badge        : " << acc->getBadgeLabel() << "\n";
    out << "Privacy      : " << (acc->getIsPrivate() ? "Private" : "Public") << "\n";
    out << "Bio          : " << acc->getBio() << "\n";
    out << "Posts        : " << postCount << "\n";
    out << "Followers    : " << followers << "\n";
    out << "Following    : " << following << "\n";
    out.close();
    return true;
}