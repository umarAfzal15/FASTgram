#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "Post.h"

class CarouselPost : public Post
{
private:
	std::vector<std::string> images; 
	static const std::size_t MAX_IMAGES = 5;

public:
	CarouselPost(); 
	CarouselPost(int authorId, const std::string& caption, const std::vector<std::string>& imagePaths);

	void addImage(const std::string& path); 
	std::size_t getImageCount() const;
	const std::string& operator[](std::size_t index) const; 

	PostType getPostType() const override;
	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};