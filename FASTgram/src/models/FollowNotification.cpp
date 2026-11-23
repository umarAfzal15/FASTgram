#include"FollowNotification.h"
using namespace std;

FollowNotification::FollowNotification() :Notification(){}
FollowNotification::FollowNotification(int id, int targetUserId, int actorUserId, const string& text, const string& timestamp) :Notification(id, targetUserId, actorUserId, text, timestamp){}

string FollowNotification::getIconType()const {
	return "user";
}

string FollowNotification::getActionRoute()const {
	return "/profile/" + to_string(actorUserId);
}
