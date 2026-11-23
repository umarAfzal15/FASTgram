#include "Account.h"
#include <cctype>
#include <cstdint>      
#include <functional>
#include <stdexcept>

int Account::nextId = 1;

namespace
{
	std::string hashPassword(const std::string& rawPassword)
	{
		std::size_t h1 = std::hash<std::string>{}("FASTagram_salt_v1_" + rawPassword);
		std::size_t h2 = std::hash<std::string>{}(rawPassword + "_FASTagram_pepper");
		return std::to_string(h1) + "_" + std::to_string(h2);
	}

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
		uint32_t len = 0;                                      // CHANGED: fixed 4 bytes
		in.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (!in || len > 1000000)
		{
			throw std::runtime_error("Corrupted string length in Account stream");
		}
		std::string str(len, '\0');
		if (len > 0)
		{
			in.read(&str[0], static_cast<std::streamsize>(len));
			if (!in)
			{
				throw std::runtime_error("Unexpected end of stream reading Account string");
			}
		}
		return str;
	}
}

Account::Account(): id(0), isPrivate(false)
{}

Account::Account(const std::string& username, const std::string& displayName, const std::string& rawPassword): id(nextId++), isPrivate(false)
{
	setUsername(username);
	setDisplayName(displayName);
	setPassword(rawPassword);
}

int Account::getId() const
{
	return id;
}

const std::string& Account::getUsername() const
{
	return username;
}

const std::string& Account::getDisplayName() const
{
	return displayName;
}

const std::string& Account::getBio() const
{
	return bio;
}

bool Account::getIsPrivate() const
{
	return isPrivate;
}

void Account::setUsername(const std::string& name)
{
	if (name.length() < 3 || name.length() > 20)
	{
		throw std::invalid_argument("Username must be between 3 and 20 characters");
	}
	for (char c : name)
	{
		if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '.')
		{
			throw std::invalid_argument("Username can only contain letters, numbers, '_' or '.'");
		}
	}
	username = name;
}

void Account::setDisplayName(const std::string& name)
{
	if (name.empty() || name.length() > 40)
	{
		throw std::invalid_argument("Display name must be between 1 and 40 characters");
	}
	displayName = name;
}

void Account::setBio(const std::string& text)
{
	if (text.length() > 160)
	{
		throw std::invalid_argument("Bio cannot exceed 160 characters");
	}
	bio = text;
}

void Account::setPassword(const std::string& rawPassword)
{
	if (rawPassword.length() < 6)
	{
		throw std::invalid_argument("Password must be at least 6 characters long");
	}
	passwordHash = hashPassword(rawPassword);
}

void Account::setPrivate(bool value)
{
	isPrivate = value;
}

bool Account::checkPassword(const std::string& rawPassword) const
{
	return passwordHash == hashPassword(rawPassword);
}

int Account::getNextId()
{
	return nextId;
}

void Account::setNextId(int value)
{
	if (value < 1)
	{
		throw std::invalid_argument("nextId must be at least 1");
	}
	nextId = value;
}

void Account::write(std::ostream& out) const
{
	out.write(reinterpret_cast<const char*>(&id), sizeof(id));
	writeString(out, username);
	writeString(out, displayName);
	writeString(out, passwordHash);
	writeString(out, bio);

	uint8_t priv = isPrivate ? 1 : 0;                         
	out.write(reinterpret_cast<const char*>(&priv), sizeof(priv));
}

void Account::read(std::istream& in)
{
	int readId = 0;
	in.read(reinterpret_cast<char*>(&readId), sizeof(readId));
	if (!in)
	{
		throw std::runtime_error("Failed to read Account from stream");
	}

	std::string readUsername = readString(in);
	std::string readDisplayName = readString(in);
	std::string readPasswordHash = readString(in);
	std::string readBio = readString(in);

	uint8_t priv = 0;

	in.read(reinterpret_cast<char*>(&priv), sizeof(priv));
	if (!in)
	{
		throw std::runtime_error("Failed to read Account from stream");
	}

	id = readId;
	username = readUsername;
	displayName = readDisplayName;
	passwordHash = readPasswordHash;
	bio = readBio;
	isPrivate = (priv != 0);

	if (id >= nextId)                                        
	{
		nextId = id + 1;
	}
}