#include "PhotoPost.h"
#include "core/BinaryIO.h"
#include <stdexcept>

PhotoPost::PhotoPost() : Post()
{}

PhotoPost::PhotoPost(int authorId, const std::string& caption, const std::string& imagePath) : Post(authorId, caption)
{
	setImagePath(imagePath);
}

const std::string& PhotoPost::getImagePath() const
{
	return imagePath;
}

void PhotoPost::setImagePath(const std::string& path)
{
	if (path.empty())
	{
		throw std::invalid_argument("PhotoPost imagePath cannot be empty");
	}
	imagePath = path;
}

PostType PhotoPost::getPostType() const
{
	return PostType::Photo;
}

void PhotoPost::write(std::ostream& out) const
{
	Post::write(out);
	BinaryIO::writeString(out, imagePath);
}

void PhotoPost::read(std::istream& in)
{
	Post::read(in);
	std::string readPath = BinaryIO::readString(in);
	imagePath = readPath;
}