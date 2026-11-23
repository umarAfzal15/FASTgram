#pragma once
#include "Account.h"

class Society : public Account
{
public:
	Society();                       
	Society(const std::string& username, const std::string& displayName, const std::string& rawPassword);

	AccountType getType() const override;
	std::string getBadgeLabel() const override;
	bool canPostNotice() const override;
	bool canDeleteAnyPost() const override;
};