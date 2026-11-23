#pragma once
#include"Notification.h"
#include<string>

class CommentNotification:public Notification{
private:
	int postId;
	int commentId;
public:
	CommentNotification();
	CommentNotification(int id, int targetUserId, int actorUserId, int postId, int commentId, const std::string& text, const std::string& timestamp);


//getters
int getPostId()const;
int getCommentId()const;

std::string getIconType()const override;
std::string getActionRoute()const override;

};