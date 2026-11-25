#pragma once
#include<vector>
#include<algorithm>

class Likeable {
protected:
	std::vector<int>likedUserIds;//used vector as it allows felxibility in size
public:
	virtual ~Likeable() = default;
	bool isLikedBy(int userId)const {
		return std::find(likedUserIds.begin(), likedUserIds.end(), userId) != likedUserIds.end();

	}

	void toggleLike(int userId) {
		auto it = std::find(likedUserIds.begin(), likedUserIds.end(), userId);
		if (it != likedUserIds.end())
			likedUserIds.erase(it);
		else
			likedUserIds.push_back(userId);
	}

	int getLikeCount()const {
		return static_cast<int>(likedUserIds.size());
	}
};