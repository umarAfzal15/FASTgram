#include "CommentNotification.h"
using namespace std;

CommentNotification::CommentNotification():Notification(),postId(-1),commentId(-1){}

CommentNotification::CommentNotification(int id,int targetUserId,int actorUserId,int postId,int commentId,const string&text,const string&timestamp):Notification(id,targetUserId,actorUserId,text,timestamp),postId(postId),commentId(commentId){}

int CommentNotification::getPostId()const {
	return postId;
}

int CommentNotification::getCommentId()const {
	return commentId;
}

string CommentNotification::getIconType()const {
	return "comment";
}

string CommentNotification::getActionRoute()const {
	return "/post/" + to_string(postId);
}