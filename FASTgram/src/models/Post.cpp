#include "Post.h"
#include "core/BinaryIO.h"
#include <algorithm>
#include <stdexcept>

int Post::nextId = 1;

Post::Post() : id(0), authorId(0), archived(false), pinned(false)
{}

Post::Post(int authorId, const std::string& caption) : id(nextId++), authorId(authorId), timestamp(Time::now()), archived(false), pinned(false)
{
	if (authorId < 1)
	{
		throw std::invalid_argument("Post authorId must be at least 1");
	}
	setCaption(caption);
}

int Post::getId() const
{
	return id;
}

int Post::getAuthorId() const
{
	return authorId;
}

std::string Post::getCaption() const
{
	return caption;
}

const Time& Post::getTimestamp() const
{
	return timestamp;
}

bool Post::isArchived() const
{
	return archived;
}

bool Post::isPinned() const
{
	return pinned;
}

void Post::setCaption(const std::string& text)
{
	if (text.length() > 2200)
	{
		throw std::invalid_argument("Caption cannot exceed 2200 characters");
	}
	caption = text;
}

void Post::setArchived(bool value)
{
	archived = value;
}

void Post::setPinned(bool value)
{
	pinned = value;
}

void Post::toggleLike(int userId)
{
	auto it = std::find(likedUserIds.begin(), likedUserIds.end(), userId);

	if (it != likedUserIds.end())
	{
		likedUserIds.erase(it);
	}
	else
	{
		likedUserIds.push_back(userId);
	}
}

bool Post::isLikedBy(int userId) const
{
	return std::find(likedUserIds.begin(), likedUserIds.end(), userId) != likedUserIds.end();
}

int Post::getLikeCount() const
{
	return static_cast<int>(likedUserIds.size());
}

bool Post::operator<(const Post& other) const
{
	if (timestamp != other.timestamp)
	{
		return timestamp < other.timestamp;
	}
	return id < other.id;
}

void Post::setNextId(int value)
{
	if (value < 1)
	{
		throw std::invalid_argument("Post nextId must be at least 1");
	}
	nextId = value;
}

void Post::write(std::ostream& out) const
{
	BinaryIO::writeInt(out, id);
	BinaryIO::writeInt(out, authorId);
	BinaryIO::writeString(out, caption);
	timestamp.write(out);
	BinaryIO::writeIntList(out, likedUserIds);
	BinaryIO::writeBool(out, archived);
	BinaryIO::writeBool(out, pinned);
}

void Post::read(std::istream& in)
{
	int readId = BinaryIO::readInt(in);
	int readAuthorId = BinaryIO::readInt(in);
	std::string readCaption = BinaryIO::readString(in);

	Time readTime;
	readTime.read(in);

	std::vector<int> readLikes = BinaryIO::readIntList(in);
	bool readArchived = BinaryIO::readBool(in);
	bool readPinned = BinaryIO::readBool(in);

	id = readId;
	authorId = readAuthorId;
	caption = readCaption;
	timestamp = readTime;
	likedUserIds = readLikes;
	archived = readArchived;
	pinned = readPinned;

	if (id >= nextId)
	{
		nextId = id + 1;
	}
}