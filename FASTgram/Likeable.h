#pragma once
#include<vector>//we r using vector here because it allows array to be flexible. can inc size etc
#include<algorithm>

class Likeable {
protected:
	std::vector<int>likedUserIds;
	
public:

	virtual ~Likeable() = default;

	bool isLikedBy(int userId)const {
		return std::find(likedUserIds.begin(), likedUserIds.end(), userId) != likedUserIds.end();//searches whole list for user id

	}

	void toggleLike(int userId) {
		auto it = std::find(likedUserIds.begin(), likedUserIds.end(), userId);
		if (it != likedUserIds.end())//if didnt reach empty end of list,
			likedUserIds.erase(it);//delete from list

		else
			likedUserIds.push_back(userId);
	}

int getLikeCount()const {
	return static_cast<int>(likedUserIds.size());
}
};
