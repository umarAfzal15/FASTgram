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
}

FileManager::FileManager(const std::string& dataFolder) : dataFolder(dataFolder)
{}

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

		out.write(MAGIC_HEADER, sizeof(MAGIC_HEADER));

		uint16_t version = FILE_VERSION;
		out.write(reinterpret_cast<const char*>(&version), sizeof(version));

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

			idxOut<<acc->getUsername()<<","<< rollStr << "," << acc->getId() << "\n";
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