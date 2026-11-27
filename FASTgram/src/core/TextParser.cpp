#include "TextParser.h"
#include <algorithm>
#include <cctype>

namespace
{
	bool isWordChar(char c)
	{
		unsigned char uc = static_cast<unsigned char>(c);
		return std::isalnum(uc) != 0 || c == '_';
	}

	std::string toLower(const std::string& text)
	{
		std::string lower = text;
		for (char& c : lower)
		{
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		return lower;
	}
}

std::vector<std::string> TextParser::extractTagged(const std::string& text, char symbol)
{
	std::vector<std::string> result;

	for (std::size_t i = 0; i < text.size(); ++i)
	{
		bool validStart = (text[i] == symbol) && (i == 0 || !std::isalnum(static_cast<unsigned char>(text[i - 1])));

		if (validStart)
		{
			std::size_t j = i + 1;
			while (j < text.size() && isWordChar(text[j]))
			{
				++j;
			}

			std::string word = text.substr(i + 1, j - i - 1);
			if (!word.empty())
			{
				word = toLower(word);
				if (std::find(result.begin(), result.end(), word) == result.end())
				{
					result.push_back(word);
				}
			}
			i = j - 1;
		}
	}

	return result;
}

std::vector<std::string> TextParser::extractHashtags(const std::string& text)
{
	return extractTagged(text, '#');
}

std::vector<std::string> TextParser::extractMentions(const std::string& text)
{
	return extractTagged(text, '@');
}