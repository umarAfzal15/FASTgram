#include "PostManager.h"
#include "core/PostFactory.h"
#include "core/BinaryIO.h"
#include "core/TextParser.h"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

PostManager::PostManager()
{
}

PostManager::~PostManager()
{
	for (Post* p : posts)
	{
		delete p;
	}
	for (PendingDelete& entry : pendingDeletes)
	{
		delete entry.post;
	}
}

void PostManager::addPost(Post* post)
{
	if (post == nullptr)
	{
		throw std::invalid_argument("Cannot add a null post");
	}
	if (findById(post->getId()) != nullptr)
	{
		throw std::invalid_argument("A post with this ID already exists");
	}
	posts.push_back(post);
}

Post* PostManager::findById(int postId) const
{
	for (Post* p : posts)
	{
		if (p->getId() == postId)
		{
			return p;
		}
	}
	return nullptr;
}

std::vector<Post*> PostManager::getPostsByUser(int userId) const
{
	std::vector<Post*> result;
	for (Post* p : posts)
	{
		if (p->getAuthorId() == userId)
		{
			result.push_back(p);
		}
	}

	std::sort(result.begin(), result.end(), [](const Post* a, const Post* b)
		{
			return *b < *a;
		});

	return result;
}

std::vector<Post*> PostManager::getPublicPosts(const UserManager& users) const
{
	std::vector<Post*> result;
	for (Post* p : posts)
	{
		if (p->isArchived())
		{
			continue;
		}

		Account* author = users.findById(p->getAuthorId());
		if (author == nullptr || author->getIsPrivate())
		{
			continue;
		}

		result.push_back(p);
	}

	std::sort(result.begin(), result.end(), [](const Post* a, const Post* b)
		{
			return *b < *a;
		});

	return result;
}

void PostManager::deletePost(int postId, const Account& requester)
{
	auto it = std::find_if(posts.begin(), posts.end(), [postId](const Post* p)
		{
			return p->getId() == postId;
		});

	if (it == posts.end())
	{
		throw std::invalid_argument("Post not found");
	}

	Post* target = *it;
	if (target->getAuthorId() != requester.getId() && !requester.canDeleteAnyPost())
	{
		throw std::runtime_error("Not allowed to delete this post");
	}

	posts.erase(it);
	pendingDeletes.push_back(PendingDelete{ target, Time::now() });
}

bool PostManager::undoDelete(int postId)
{
	for (std::size_t i = 0; i < pendingDeletes.size(); ++i)
	{
		if (pendingDeletes[i].post->getId() == postId)
		{
			if (Time::now() - pendingDeletes[i].deletedAt <= UNDO_SECONDS)
			{
				Post* restored = pendingDeletes[i].post;
				pendingDeletes.erase(pendingDeletes.begin() + static_cast<std::ptrdiff_t>(i));
				posts.push_back(restored);
				return true;
			}
			return false;
		}
	}
	return false;
}

void PostManager::purgeExpired()
{
	std::size_t i = 0;
	while (i < pendingDeletes.size())
	{
		if (Time::now() - pendingDeletes[i].deletedAt > UNDO_SECONDS)
		{
			delete pendingDeletes[i].post;
			pendingDeletes.erase(pendingDeletes.begin() + static_cast<std::ptrdiff_t>(i));
		}
		else
		{
			++i;
		}
	}
}


void PostManager::save(const std::string& dataFolder) const
{
	fs::path dir(dataFolder);
	fs::create_directories(dir);

	fs::path finalFile = dir / "posts.dat";
	fs::path tempFile = dir / "posts.dat.tmp";

	std::ofstream out(tempFile, std::ios::binary | std::ios::trunc);
	if (!out)
	{
		throw std::runtime_error("Failed to open posts.dat.tmp for writing");
	}

	BinaryIO::writeHeader(out, static_cast<uint32_t>(posts.size()));

	for (const Post* p : posts)
	{
		std::ostringstream payload(std::ios::binary);
		p->write(payload);
		std::string bytes = payload.str();

		uint8_t tag = static_cast<uint8_t>(p->getPostType());
		uint32_t size = static_cast<uint32_t>(bytes.size());

		out.write(reinterpret_cast<const char*>(&tag), sizeof(tag));
		out.write(reinterpret_cast<const char*>(&size), sizeof(size));
		if (size > 0)
		{
			out.write(bytes.data(), static_cast<std::streamsize>(size));
		}
	}

	out.flush();
	if (!out)
	{
		out.close();
		throw std::runtime_error("Failed while writing posts.dat.tmp");
	}
	out.close();

	fs::rename(tempFile, finalFile);

	saveTagsFile(dataFolder);
}

void PostManager::load(const std::string& dataFolder)
{
	fs::path filePath = fs::path(dataFolder) / "posts.dat";
	if (!fs::exists(filePath))
	{
		return;
	}

	std::ifstream in(filePath, std::ios::binary);
	if (!in)
	{
		throw std::runtime_error("Failed to open posts.dat for reading");
	}

	uint32_t count = BinaryIO::readHeader(in);

	for (uint32_t i = 0; i < count; ++i)
	{
		uint8_t tag = 0;
		uint32_t size = 0;

		in.read(reinterpret_cast<char*>(&tag), sizeof(tag));
		in.read(reinterpret_cast<char*>(&size), sizeof(size));
		if (!in || size > 1000000)
		{
			throw std::runtime_error("Damaged post record header in posts.dat");
		}

		std::string bytes(size, '\0');
		if (size > 0)
		{
			in.read(&bytes[0], static_cast<std::streamsize>(size));
			if (!in)
			{
				throw std::runtime_error("Unexpected end of stream reading post payload");
			}
		}

		Post* p = PostFactory::createBlank(tag);
		try
		{
			std::istringstream payload(bytes, std::ios::binary);
			p->read(payload);
			addPost(p);
		}
		catch (...)
		{
			delete p;
			throw;
		}
	}
}

void PostManager::saveTagsFile(const std::string& dataFolder) const
{
	std::map<std::string, std::vector<int>> tagIndex;

	for (const Post* p : posts)
	{
		std::vector<std::string> hashtags = TextParser::extractHashtags(p->getCaption());
		for (const std::string& tag : hashtags)
		{
			tagIndex[tag].push_back(p->getId());
		}
	}

	fs::path dir(dataFolder);
	fs::path finalFile = dir / "tags.txt";
	fs::path tempFile = dir / "tags.txt.tmp";

	std::ofstream out(tempFile, std::ios::trunc);
	if (!out)
	{
		throw std::runtime_error("Failed to open tags.txt.tmp for writing");
	}

	for (const auto& pair : tagIndex)
	{
		out << pair.first;
		for (int postId : pair.second)
		{
			out << ' ' << postId;
		}
		out << '\n';
	}

	out.flush();
	if (!out)
	{
		out.close();
		throw std::runtime_error("Failed while writing tags.txt.tmp");
	}
	out.close();

	fs::rename(tempFile, finalFile);
}