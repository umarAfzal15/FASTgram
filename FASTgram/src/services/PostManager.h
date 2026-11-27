#pragma once
#include <string>
#include <vector>
#include "core/Time.h"
#include "models/Post.h"
#include "models/Account.h"
#include "UserManager.h"

class PostManager
{
private:
	struct PendingDelete
	{
		Post* post;
		Time deletedAt;
	};

	std::vector<Post*> posts;                
	std::vector<PendingDelete> pendingDeletes; 
	static constexpr long long UNDO_SECONDS = 5;

	void saveTagsFile(const std::string& dataFolder) const;

public:
	PostManager();
	~PostManager();

	PostManager(const PostManager&) = delete;
	PostManager& operator=(const PostManager&) = delete;

	void addPost(Post* post);                                        
	Post* findById(int postId) const;
	std::vector<Post*> getPostsByUser(int userId) const;              
	std::vector<Post*> getPublicPosts(const UserManager& users) const; 

	void deletePost(int postId, const Account& requester);             
	bool undoDelete(int postId);
	void purgeExpired();                                               

	void save(const std::string& dataFolder) const;                   
	void load(const std::string& dataFolder);                        
};