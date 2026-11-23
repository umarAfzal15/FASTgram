#pragma once
#include"Notification.h"
#include<string>

class LikeNotification :public Notification {
private:
	int postId;

public:
	LikeNotification();
	LikeNotification(int id, int targetUserId, int actorUserId, int postId, const std::string& text, const std::string& timestamp);
	int getPostId()const;//getter fn of postid

	std::string getIconType()const override;//implementing virtual fn from Notificaiton
	std::string getActionRoute()const override;
};
