#include "PostFactory.h"
#include "models/PhotoPost.h"
#include "models/TextPost.h"
#include "models/CarouselPost.h"
#include "models/PollPost.h"
#include <stdexcept>

Post* PostFactory::createBlank(uint8_t tag)
{
	switch (static_cast<PostType>(tag))
	{
	case PostType::Photo:
		return new PhotoPost();
	case PostType::Text:
		return new TextPost();
	case PostType::Carousel:
		return new CarouselPost();
	case PostType::Poll:
		return new PollPost();
	default:
		throw std::runtime_error("Damaged file: unknown post type tag");
	}
}

Post* PostFactory::create(PostType type, const PostData& data)
{
	switch (type)
	{
	case PostType::Photo:
		return new PhotoPost(data.authorId, data.caption, data.imagePath);
	case PostType::Text:
		return new TextPost(data.authorId, data.caption);
	case PostType::Carousel:
		return new CarouselPost(data.authorId, data.caption, data.images);
	case PostType::Poll:
		return new PollPost(data.authorId, data.caption, data.question, data.options);
	default:
		throw std::invalid_argument("Unknown post type");
	}
}