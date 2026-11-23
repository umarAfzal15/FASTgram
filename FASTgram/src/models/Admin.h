#pragma once
#include "Account.h"

class Admin : public Account
{
public:
	Admin();                       // empty, used when loading from file
	Admin(const std::string& username,
		const std::string& displayName,
		const std::string& rawPassword);

	AccountType getType() const override;
	std::string getBadgeLabel() const override;
	bool canPostNotice() const override;
	bool canDeleteAnyPost() const override;
};