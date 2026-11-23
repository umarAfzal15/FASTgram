#include "Session.h"
#include <stdexcept>

Session::Session() : currentUserId(-1) {}

void Session::login(int userId)
{
	if (userId < 0)
	{
		throw std::invalid_argument("User ID cannot be negative");
	}
	currentUserId = userId;

}

void Session::logout()
{
	currentUserId = -1;
}

bool Session::isLoggedIn() const
{
	return currentUserId != -1;
}

int Session::getCurrentUserId() const
{
	return currentUserId;
}