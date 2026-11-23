#include "Admin.h"

Admin::Admin()
	: Account()
{
}

Admin::Admin(const std::string& username,
	const std::string& displayName,
	const std::string& rawPassword)
	: Account(username, displayName, rawPassword)
{
}

AccountType Admin::getType() const
{
	return AccountType::Admin;
}

std::string Admin::getBadgeLabel() const
{
	return "Admin";
}

bool Admin::canPostNotice() const
{
	return true;
}

bool Admin::canDeleteAnyPost() const
{
	return true;
}