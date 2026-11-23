#pragma once
#include <cstdint>
#include <string>
#include "UserManager.h"

class FileManager
{
private:
	std::string dataFolder;                      
	Account* createAccount(uint8_t tag) const;  

public:
	explicit FileManager(const std::string& dataFolder = "data");

	void saveUsers(const UserManager& users) const;   
	void loadUsers(UserManager& users) const;         
};