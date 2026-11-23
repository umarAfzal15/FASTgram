#pragma once
#include<string>
class Notification {
protected:
	int id;
	int targetUserId;
	int actorUserId;
	std::string text;
	bool isRead;
	std::string timestamp;
	
public:

	Notification(); //default const
	Notification(int id, int targetUserId, int actorUserId, const std::string& text, const std::string& timestamp);
	virtual ~Notification() = default;

	int getId()const;
	int getTargetUserId()const;
	int getActorUserId()const;
	std::string getText()const;
	bool getIsRead()const;
	std::string getTimestamp()const;
	
	
	void markAsRead();
	virtual std::string getIconType() const = 0;//these r pure virtual fns.
	virtual std::string getActionRoute()const = 0;
};