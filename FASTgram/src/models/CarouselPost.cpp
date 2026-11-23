#include "CarouselPost.h"
#include "core/BinaryIO.h"
#include <stdexcept>

CarouselPost::CarouselPost() : Post()
{}

CarouselPost::CarouselPost(int authorId, const std::string& caption, const std::vector<std::string>& imagePaths) : Post(authorId, caption)
{
	if (imagePaths.empty())
	{
		throw std::invalid_argument("CarouselPost requires at least 1 image");
	}
	for (const std::string& path : imagePaths)
	{
		addImage(path);
	}
}

void CarouselPost::addImage(const std::string& path)
{
	if (path.empty())
	{
		throw std::invalid_argument("CarouselPost image path cannot be empty");
	}
	if (images.size() >= MAX_IMAGES)
	{
		throw std::invalid_argument("CarouselPost cannot exceed 5 images");
	}
	images.push_back(path);
}

std::size_t CarouselPost::getImageCount() const
{
	return images.size();
}

const std::string& CarouselPost::operator[](std::size_t index) const
{
	if (index >= images.size())
	{
		throw std::out_of_range("CarouselPost image index out of range");
	}
	return images[index];
}

PostType CarouselPost::getPostType() const
{
	return PostType::Carousel;
}

void CarouselPost::write(std::ostream& out) const
{
	Post::write(out);
	BinaryIO::writeStringList(out, images);
}

void CarouselPost::read(std::istream& in)
{
	Post::read(in);
	std::vector<std::string> readImages = BinaryIO::readStringList(in);
	if (readImages.empty() || readImages.size() > MAX_IMAGES)
	{
		throw std::runtime_error("Corrupted CarouselPost image count in stream");
	}
	images = readImages;
}