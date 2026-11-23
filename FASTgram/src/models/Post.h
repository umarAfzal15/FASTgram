#pragma once
#include <string>
#include <vector>
#include "core/Serializable.h"
#include "core/Time.h"

enum class PostType
{
	Photo = 0,
	Text = 1,
	Carousel = 2,
	Poll = 3
};

class Post : public Serializable
{
private:
	int id;
	int authorId;
	std::string caption;
	Time timestamp;
	std::vector<int> likedUserIds;
	bool archived;
	bool pinned;
	static int nextId;

protected:
	Post();

public:
	Post(int authorId, const std::string& caption);
	virtual ~Post() = default;

	int getId() const;
	int getAuthorId() const;
	std::string getCaption() const;
	const Time& getTimestamp() const;
	bool isArchived() const;
	bool isPinned() const;

	virtual void setCaption(const std::string& text);
	void setArchived(bool value);
	void setPinned(bool value);

	void toggleLike(int userId);
	bool isLikedBy(int userId) const;
	int getLikeCount() const;

	bool operator<(const Post& other) const;

	virtual PostType getPostType() const = 0;
	static void setNextId(int value);

	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};