#pragma once
#include <string>
#include <vector>

class TextParser {
public:
    // Feature P6: Extract #hashtags and @mentions from any caption string
    static std::vector<std::string> extractHashtags(const std::string& text);
    static std::vector<std::string> extractMentions(const std::string& text);
};