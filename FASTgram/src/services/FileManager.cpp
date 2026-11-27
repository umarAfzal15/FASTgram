#include "FileManager.h"
#include "models/Student.h"
#include "models/Faculty.h"
#include "models/Society.h"
#include "models/Admin.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace
{
	constexpr char MAGIC_HEADER[4] = { 'F', 'T', 'G', '1' };
	constexpr uint16_t FILE_VERSION = 1;
	constexpr uint32_t MAX_RECORD_SIZE = 1000000;

	void writeString(std::ostream& out, const std::string& str)
	{
		uint32_t len = static_cast<uint32_t>(str.size());
		out.write(reinterpret_cast<const char*>(&len), sizeof(len));
		if (len > 0)
		{
			out.write(str.data(), static_cast<std::streamsize>(len));
		}
	}

	std::string readString(std::istream& in)
	{
		uint32_t len = 0;
		in.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (!in || len > MAX_RECORD_SIZE)
		{
			throw std::runtime_error("Damaged file: invalid string length");
		}
		std::string str(len, '\0');
		if (len > 0)
		{
			in.read(&str[0], static_cast<std::streamsize>(len));
			if (!in)
			{
				throw std::runtime_error("Damaged file: unexpected EOF reading string");
			}
		}
		return str;
	}

	void writeHeader(std::ostream& out)
	{
		out.write(MAGIC_HEADER, sizeof(MAGIC_HEADER));
		uint16_t version = FILE_VERSION;
		out.write(reinterpret_cast<const char*>(&version), sizeof(version));
	}

	void verifyHeader(std::istream& in)
	{
		char magic[4] = {};
		in.read(magic, sizeof(magic));
		if (!in || std::string(magic, 4) != std::string(MAGIC_HEADER, 4))
		{
			throw std::runtime_error("Wrong or damaged file: invalid FTG1 header");
		}

		uint16_t version = 0;
		in.read(reinterpret_cast<char*>(&version), sizeof(version));
		if (!in || version != FILE_VERSION)
		{
			throw std::runtime_error("Wrong or damaged file: unsupported version");
		}
	}
}

FileManager::FileManager(const std::string& dataFolder) : dataFolder(dataFolder)
{
}

Account* FileManager::createAccount(uint8_t tag) const
{
	switch (tag)
	{
	case 0:
		return new Student();
	case 1:
		return new Faculty();
	case 2:
		return new Society();
	case 3:
		return new Admin();
	default:
		throw std::runtime_error("Damaged file: unknown account type tag");
	}
}

void FileManager::saveUsers(const UserManager& users) const
{
	fs::create_directories(dataFolder);

	const fs::path folderPath(dataFolder);
	const fs::path datFinal = folderPath / "users.dat";
	const fs::path datTmp = folderPath / "users.dat.tmp";

	{
		std::ofstream out(datTmp, std::ios::binary | std::ios::trunc);
		if (!out)
		{
			throw std::runtime_error("Failed to open temporary file for users.dat");
		}

		writeHeader(out);

		const std::vector<Account*>& allAccounts = users.getAllAccounts();
		uint32_t count = static_cast<uint32_t>(allAccounts.size());
		out.write(reinterpret_cast<const char*>(&count), sizeof(count));

		for (const Account* acc : allAccounts)
		{
			std::ostringstream recordStream(std::ios::binary);
			acc->write(recordStream);
			std::string payload = recordStream.str();

			uint8_t tag = static_cast<uint8_t>(acc->getType());
			uint32_t size = static_cast<uint32_t>(payload.size());

			out.write(reinterpret_cast<const char*>(&tag), sizeof(tag));
			out.write(reinterpret_cast<const char*>(&size), sizeof(size));
			out.write(payload.data(), static_cast<std::streamsize>(payload.size()));
		}

		out.flush();
		if (!out)
		{
			throw std::runtime_error("Error occurred while writing users.dat.tmp");
		}
	}

	fs::rename(datTmp, datFinal);

	const fs::path idxFinal = folderPath / "users_index.txt";
	const fs::path idxTmp = folderPath / "users_index.txt.tmp";

	{
		std::ofstream idxOut(idxTmp, std::ios::trunc);
		if (!idxOut)
		{
			throw std::runtime_error("Failed to open temporary file for users_index.txt");
		}

		for (const Account* acc : users.getAllAccounts())
		{
			const Student* stu = dynamic_cast<const Student*>(acc);
			std::string rollStr = (stu != nullptr) ? stu->getRollNumber().toString() : "-";

			idxOut << acc->getUsername() << "," << rollStr << "," << acc->getId() << "\n";
		}

		idxOut.flush();
		if (!idxOut)
		{
			throw std::runtime_error("Error occurred while writing users_index.txt.tmp");
		}
	}

	fs::rename(idxTmp, idxFinal);
}

void FileManager::loadUsers(UserManager& users) const
{
	const fs::path datPath = fs::path(dataFolder) / "users.dat";
	if (!fs::exists(datPath))
	{
		return;
	}

	std::ifstream in(datPath, std::ios::binary);
	if (!in)
	{
		throw std::runtime_error("Failed to open users.dat for reading");
	}

	verifyHeader(in);

	uint32_t count = 0;
	in.read(reinterpret_cast<char*>(&count), sizeof(count));
	if (!in)
	{
		throw std::runtime_error("Damaged file: could not read account count");
	}

	for (uint32_t i = 0; i < count; ++i)
	{
		uint8_t tag = 0;
		in.read(reinterpret_cast<char*>(&tag), sizeof(tag));
		if (!in)
		{
			throw std::runtime_error("Damaged file: unexpected EOF reading account tag");
		}

		uint32_t size = 0;
		in.read(reinterpret_cast<char*>(&size), sizeof(size));
		if (!in)
		{
			throw std::runtime_error("Damaged file: unexpected EOF reading record size");
		}

		if (size > MAX_RECORD_SIZE)
		{
			throw std::runtime_error("Damaged file: record size too large");
		}

		std::string payload(size, '\0');
		if (size > 0)
		{
			in.read(&payload[0], static_cast<std::streamsize>(size));
			if (!in)
			{
				throw std::runtime_error("Damaged file: unexpected EOF reading record bytes");
			}
		}

		Account* account = createAccount(tag);
		try
		{
			std::istringstream recordStream(payload, std::ios::binary);
			account->read(recordStream);
			users.addAccount(account);
		}
		catch (...)
		{
			delete account;
			throw;
		}
	}
}

void FileManager::saveComments(const std::vector<Comment*>& comments) const
{
	fs::create_directories(dataFolder);

	const fs::path folderPath(dataFolder);
	const fs::path datFinal = folderPath / "comments.dat";
	const fs::path datTmp = folderPath / "comments.dat.tmp";

	{
		std::ofstream out(datTmp, std::ios::binary | std::ios::trunc);
		if (!out)
		{
			throw std::runtime_error("Failed to open temporary file for comments.dat");
		}

		writeHeader(out);

		uint32_t count = 0;
		for (int i = 0; i < static_cast<int>(comments.size()); i++)
		{
			if (comments[i] != nullptr)
			{
				count++;
			}
		}
		out.write(reinterpret_cast<const char*>(&count), sizeof(count));

		for (int i = 0; i < static_cast<int>(comments.size()); i++)
		{
			const Comment* c = comments[i];
			if (c == nullptr) continue;

			int commentId = c->getCommentId();
			int authorId = c->getAuthorId();
			out.write(reinterpret_cast<const char*>(&commentId), sizeof(commentId));
			out.write(reinterpret_cast<const char*>(&authorId), sizeof(authorId));
			writeString(out, c->getText());
			writeString(out, c->getTimestamp());
		}

		out.flush();
		if (!out)
		{
			throw std::runtime_error("Error occurred while writing comments.dat.tmp");
		}
	}

	fs::rename(datTmp, datFinal);
}

void FileManager::loadComments(std::vector<Comment*>& comments) const
{
	const fs::path datPath = fs::path(dataFolder) / "comments.dat";
	if (!fs::exists(datPath))
	{
		return;
	}

	std::ifstream in(datPath, std::ios::binary);
	if (!in)
	{
		throw std::runtime_error("Failed to open comments.dat for reading");
	}

	verifyHeader(in);

	uint32_t count = 0;
	in.read(reinterpret_cast<char*>(&count), sizeof(count));
	if (!in)
	{
		throw std::runtime_error("Damaged file: could not read comment count");
	}

	for (uint32_t i = 0; i < count; ++i)
	{
		int commentId = 0;
		int authorId = 0;
		in.read(reinterpret_cast<char*>(&commentId), sizeof(commentId));
		in.read(reinterpret_cast<char*>(&authorId), sizeof(authorId));
		if (!in)
		{
			throw std::runtime_error("Damaged file: unexpected EOF reading comment IDs");
		}

		std::string text = readString(in);
		std::string timestamp = readString(in);

		comments.push_back(new Comment(commentId, authorId, text, timestamp));
	}
}

void FileManager::saveSocialGraph(const SocialGraph& graph) const
{
	fs::create_directories(dataFolder);

	const fs::path folderPath(dataFolder);
	const fs::path datFinal = folderPath / "follows.dat";
	const fs::path datTmp = folderPath / "follows.dat.tmp";

	{
		std::ofstream out(datTmp, std::ios::binary | std::ios::trunc);
		if (!out)
		{
			throw std::runtime_error("Failed to open temporary file for follows.dat");
		}

		writeHeader(out);

		const auto& follows = graph.getFollowEdges();
		uint32_t followCount = static_cast<uint32_t>(follows.size());
		out.write(reinterpret_cast<const char*>(&followCount), sizeof(followCount));
		for (int i = 0; i < static_cast<int>(follows.size()); i++)
		{
			int a = follows[i].first;
			int b = follows[i].second;
			out.write(reinterpret_cast<const char*>(&a), sizeof(a));
			out.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}

		const auto& pending = graph.getPendingRequestEdges();
		uint32_t pendingCount = static_cast<uint32_t>(pending.size());
		out.write(reinterpret_cast<const char*>(&pendingCount), sizeof(pendingCount));
		for (int i = 0; i < static_cast<int>(pending.size()); i++)
		{
			int a = pending[i].first;
			int b = pending[i].second;
			out.write(reinterpret_cast<const char*>(&a), sizeof(a));
			out.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}

		const auto& blocked = graph.getBlockedEdges();
		uint32_t blockedCount = static_cast<uint32_t>(blocked.size());
		out.write(reinterpret_cast<const char*>(&blockedCount), sizeof(blockedCount));
		for (int i = 0; i < static_cast<int>(blocked.size()); i++)
		{
			int a = blocked[i].first;
			int b = blocked[i].second;
			out.write(reinterpret_cast<const char*>(&a), sizeof(a));
			out.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}

		const auto& muted = graph.getMutedEdges();
		uint32_t mutedCount = static_cast<uint32_t>(muted.size());
		out.write(reinterpret_cast<const char*>(&mutedCount), sizeof(mutedCount));
		for (int i = 0; i < static_cast<int>(muted.size()); i++)
		{
			int a = muted[i].first;
			int b = muted[i].second;
			out.write(reinterpret_cast<const char*>(&a), sizeof(a));
			out.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}

		out.flush();
		if (!out)
		{
			throw std::runtime_error("Error occurred while writing follows.dat.tmp");
		}
	}

	fs::rename(datTmp, datFinal);
}

void FileManager::loadSocialGraph(SocialGraph& graph) const
{
	const fs::path datPath = fs::path(dataFolder) / "follows.dat";
	if (!fs::exists(datPath))
	{
		return;
	}

	std::ifstream in(datPath, std::ios::binary);
	if (!in)
	{
		throw std::runtime_error("Failed to open follows.dat for reading");
	}

	verifyHeader(in);

	uint32_t followCount = 0;
	in.read(reinterpret_cast<char*>(&followCount), sizeof(followCount));
	if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading follow count");
	for (uint32_t i = 0; i < followCount; ++i)
	{
		int a = 0, b = 0;
		in.read(reinterpret_cast<char*>(&a), sizeof(a));
		in.read(reinterpret_cast<char*>(&b), sizeof(b));
		if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading follow edge");
		graph.addLoadedFollow(a, b);
	}

	uint32_t pendingCount = 0;
	in.read(reinterpret_cast<char*>(&pendingCount), sizeof(pendingCount));
	if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading pending count");
	for (uint32_t i = 0; i < pendingCount; ++i)
	{
		int a = 0, b = 0;
		in.read(reinterpret_cast<char*>(&a), sizeof(a));
		in.read(reinterpret_cast<char*>(&b), sizeof(b));
		if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading pending edge");
		graph.addLoadedPendingRequest(a, b);
	}

	uint32_t blockedCount = 0;
	in.read(reinterpret_cast<char*>(&blockedCount), sizeof(blockedCount));
	if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading blocked count");
	for (uint32_t i = 0; i < blockedCount; ++i)
	{
		int a = 0, b = 0;
		in.read(reinterpret_cast<char*>(&a), sizeof(a));
		in.read(reinterpret_cast<char*>(&b), sizeof(b));
		if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading blocked edge");
		graph.addLoadedBlock(a, b);


		uint32_t mutedCount = 0;
		in.read(reinterpret_cast<char*>(&mutedCount), sizeof(mutedCount));
		if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading muted count");
		for (uint32_t i = 0; i < mutedCount; ++i)
		{
			int a = 0, b = 0;
			in.read(reinterpret_cast<char*>(&a), sizeof(a));
			in.read(reinterpret_cast<char*>(&b), sizeof(b));
			if (!in) throw std::runtime_error("Damaged file: unexpected EOF reading muted edge");
			graph.addLoadedMute(a, b);
		}
	}
}