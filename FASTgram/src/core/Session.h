#pragma once

class Session
{
private:
	int currentUserId;  // when '-1' it means that no user is logged in  

public:
	Session();

	void login(int userId);
	void logout();

	bool isLoggedIn() const;
	int getCurrentUserId() const;
};