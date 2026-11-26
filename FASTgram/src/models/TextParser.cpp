#include "TextParser.h"
#include <cctype>

static bool isTokenChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::vector<std::string> TextParser::extractHashtags(const std::string& text) {
    std::vector<std::string> hashtags;
    int n = static_cast<int>(text.size());

    for (int i = 0; i < n; i++) {
        if (text[i] == '#') {
            int start = i;
            int j = i + 1;
            while (j < n && isTokenChar(text[j])) {
                j++;
            }
            if (j > start + 1) {
                std::string tag = text.substr(start, j - start);
                bool exists = false;
                for (int k = 0; k < static_cast<int>(hashtags.size()); k++) {
                    if (hashtags[k] == tag) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    hashtags.push_back(tag);
                }
            }
            i = j - 1;
        }
    }
    return hashtags;
}

std::vector<std::string> TextParser::extractMentions(const std::string& text) {
    std::vector<std::string> mentions;
    int n = static_cast<int>(text.size());

    for (int i = 0; i < n; i++) {
        if (text[i] == '@') {
            int start = i;
            int j = i + 1;
            while (j < n && isTokenChar(text[j])) {
                j++;
            }
            if (j > start + 1) {
                std::string mention = text.substr(start, j - start);
                bool exists = false;
                for (int k = 0; k < static_cast<int>(mentions.size()); k++) {
                    if (mentions[k] == mention) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    mentions.push_back(mention);
                }
            }
            i = j - 1;
        }
    }
    return mentions;
}