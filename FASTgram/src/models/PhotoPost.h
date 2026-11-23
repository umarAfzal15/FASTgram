#pragma once
#include <string>
#include "Post.h"

class PhotoPost : public Post
{
private:
	std::string imagePath;

public:
	PhotoPost(); 
	PhotoPost(int authorId, const std::string& caption, const std::string& imagePath);

	const std::string& getImagePath() const;
	void setImagePath(const std::string& path);

	PostType getPostType() const override;
	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};