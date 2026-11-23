#include "UserManager.h"
#include "models/Student.h"
#include "models/Faculty.h"
#include "models/Society.h"
#include <cctype>
#include <stdexcept>

namespace
{
	std::string toLower(const std::string& text)
	{
		std::string result = text;
		for (char& c : result)
		{
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		return result;
	}
}

UserManager::UserManager()
{}

UserManager::~UserManager()
{
	for (Account* a : accounts)
	{
		delete a;
	}
	accounts.clear();
}

bool UserManager::usernameExists(const std::string& username) const
{
	return findByUsername(username) != nullptr;
}

Account* UserManager::findByUsername(const std::string& username) const
{
	std::string target = toLower(username);
	for (Account* a : accounts)
	{
		if (toLower(a->getUsername()) == target)
		{
			return a;
		}
	}
	return nullptr;
}

Account* UserManager::findById(int id) const
{
	for (Account* a : accounts)
	{
		if (a->getId() == id)
		{
			return a;
		}
	}
	return nullptr;
}

void UserManager::addAccount(Account* account)
{
	if (account == nullptr)
	{
		throw std::invalid_argument("Cannot add a null account.");
	}
	accounts.push_back(account);
}

const std::vector<Account*>& UserManager::getAllAccounts() const
{
	return accounts;
}

bool UserManager::rollNumberExists(const RollNumber& roll) const
{
	for (Account* a : accounts)
	{
		Student* s = dynamic_cast<Student*>(a);   
		if (s != nullptr && s->getRollNumber() == roll)
		{
			return true;
		}
	}
	return false;
}

bool UserManager::registerStudent(const std::string& username, const std::string& displayName,
	const std::string& password, const std::string& rollNo)
{
	RollNumber roll(rollNo);

	if (usernameExists(username))
	{
		throw std::invalid_argument("Username already taken");
	}

	if (rollNumberExists(roll))
	{
		throw std::invalid_argument("Roll number already registered");
	}

	accounts.push_back(new Student(username, displayName, password, roll));
	return true;
}

bool UserManager::registerStaffOrSociety(AccountType type, const std::string& username, const std::string& displayName, const std::string& password)
{
	if (usernameExists(username))
	{
		throw std::invalid_argument("Username already taken");
	}

	if (type == AccountType::Faculty)
	{
		accounts.push_back(new Faculty(username, displayName, password));
	}
	else if (type == AccountType::Society)
	{
		accounts.push_back(new Society(username, displayName, password));
	}
	else
	{
		throw std::invalid_argument("Invalid account type for staff or society registration");
	}
	return true;
}

Account* UserManager::login(const std::string& username, const std::string& password)
{
	Account* acc = findByUsername(username);
	if (acc == nullptr || !acc->checkPassword(password))
	{
		throw std::invalid_argument("Invalid username or password");
	}
	return acc;
}