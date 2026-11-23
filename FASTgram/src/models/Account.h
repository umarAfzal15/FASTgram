#pragma once
#include <string>
#include "core/Serializable.h"

enum class AccountType { Student = 0, Faculty = 1, Society = 2, Admin = 3};

class Account : public Serializable
{
private:
	int id;
	std::string username;
	std::string displayName;
	std::string passwordHash;
	std::string bio;
	bool isPrivate;

	static int nextId;                       


protected:
	Account();                               

public:
	Account(const std::string& username, const std::string& displayName, const std::string& rawPassword);
	virtual ~Account() = default;

	int getId() const;
	const std::string& getUsername() const;
	const std::string& getDisplayName() const;
	const std::string& getBio() const;
	bool getIsPrivate() const;

	void setUsername(const std::string& name);
	void setDisplayName(const std::string& name);
	void setBio(const std::string& text);
	void setPassword(const std::string& rawPassword);
	void setPrivate(bool value);

	bool checkPassword(const std::string& rawPassword) const;

	virtual AccountType getType() const = 0;
	virtual std::string getBadgeLabel() const = 0;
	virtual bool canPostNotice() const = 0;
	virtual bool canDeleteAnyPost() const = 0;

	static int getNextId();
	static void setNextId(int value);        

	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};