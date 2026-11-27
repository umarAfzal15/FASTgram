#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "UserManager.h"
#include "models/Comment.h"
#include "models/SocialGraph.h"

class FileManager
{
private:
	std::string dataFolder;
	Account* createAccount(uint8_t tag) const;

public:
	explicit FileManager(const std::string& dataFolder = "data");

	void saveUsers(const UserManager& users) const;
	void loadUsers(UserManager& users) const;

	// Day 4: Binary persistence for comments.dat & follows.dat
	void saveComments(const std::vector<Comment*>& comments) const;
	void loadComments(std::vector<Comment*>& comments) const;

	void saveSocialGraph(const SocialGraph& graph) const;
	void loadSocialGraph(SocialGraph& graph) const;
};