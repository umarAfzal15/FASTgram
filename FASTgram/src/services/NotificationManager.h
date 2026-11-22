#pragma once
#include "../models/Notification.h"

class NotificationManager {
private:
	Notification** list;
	int count;
	int capacity;

	void resize();

public:
	NotificationManager();//defualt cons
	~NotificationManager();//detsurctor

	void addNotification(Notification* notif);
	Notification** getForUser(int userId, int& outCount)const;
	int getUnreadCount(int userId)const;
	void markAllRead(int userId);//doesnt return nything
	void clearForUser(int userId);

};