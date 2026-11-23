#pragma once
#include"Notification.h"
#include<string>

class FollowNotification:public Notification {
public:
	FollowNotification();
	FollowNotification(int id, int targetUserId, int actorUserId, const std::string& text, const std::string& timestamp);
	std::string getIconType()const override;
	std::string getActionRoute()const override;

};