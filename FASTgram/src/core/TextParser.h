#pragma once
#include <string>
#include <vector>

class TextParser
{
private:
	static std::vector<std::string> extractTagged(const std::string& text, char symbol);

public:
	static std::vector<std::string> extractHashtags(const std::string& text);
	static std::vector<std::string> extractMentions(const std::string& text);
};