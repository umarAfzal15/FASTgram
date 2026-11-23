#pragma once
#include <string>
#include <vector>
#include "models/Account.h"
#include "core/RollNumber.h"

class UserManager
{
private:
	std::vector<Account*> accounts;     

	bool usernameExists(const std::string& username) const;
	bool rollNumberExists(const RollNumber& roll) const;

public:
	UserManager();
	~UserManager();

	UserManager(const UserManager&) = delete;              
	UserManager& operator=(const UserManager&) = delete;

	Account* login(const std::string& username, const std::string& password);

	bool registerStudent(const std::string& username, const std::string& displayName, const std::string& password, const std::string& rollNo);
	
	bool registerStaffOrSociety(AccountType type, const std::string& username, const std::string& displayName, const std::string& password);
	
	Account* findById(int id) const;


	Account* findByUsername(const std::string& username) const;

	void addAccount(Account* account);                   

	const std::vector<Account*>& getAllAccounts() const;
};