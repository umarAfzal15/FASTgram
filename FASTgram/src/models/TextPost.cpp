#include "TextPost.h"
#include <stdexcept>

TextPost::TextPost() : Post()
{}

TextPost::TextPost(int authorId, const std::string& text) : Post(authorId, text)
{
	setCaption(text);
}

void TextPost::setCaption(const std::string& text)
{
	if (text.empty() || text.length() > 200)
	{
		throw std::invalid_argument("TextPost text must be between 1 and 200 characters");
	}
	Post::setCaption(text);
}

PostType TextPost::getPostType() const
{
	return PostType::Text;
}