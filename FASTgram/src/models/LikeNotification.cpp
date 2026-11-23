#include "LikeNotification.h" //inherits all common fields from notification.h

using namespace std;
LikeNotification::LikeNotification():Notification(),postId(-1){}

LikeNotification::LikeNotification(int id, int targetUserId, int actorUserId, int postId, const string& text, const string& timestamp)
	: Notification(id,targetUserId, actorUserId, text,timestamp), postId(postId) { //passes 5 share dvalues to notfication
}

int LikeNotification::getPostId()const {
	return postId;
}

string LikeNotification::getIconType()const {
	return "heart";
}
string LikeNotification::getActionRoute()const {
	return "/post/" + to_string(postId);
}
