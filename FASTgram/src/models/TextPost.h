#pragma once
#include <string>
#include "Post.h"

class TextPost : public Post
{
public:
	TextPost(); 
	TextPost(int authorId, const std::string& text);

	void setCaption(const std::string& text) override; 

	PostType getPostType() const override;
};