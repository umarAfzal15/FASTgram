#include "Society.h"

Society::Society() : Account()
{}

Society::Society(const std::string& username, const std::string& displayName, const std::string& rawPassword) : Account(username, displayName, rawPassword)
{}

AccountType Society::getType() const
{
	return AccountType::Society;
}

std::string Society::getBadgeLabel() const
{
	return "Society";
}

bool Society::canPostNotice() const
{
	return true;
}

bool Society::canDeleteAnyPost() const
{
	return false;
}