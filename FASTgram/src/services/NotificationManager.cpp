#include"NotificationManager.h"
using namespace std;

NotificationManager::NotificationManager() {
	capacity = 10;
	count = 0;
	list = new Notification * [capacity];

}

NotificationManager::~NotificationManager() {
	for (int i = 0;i < count;i++) {
		delete list[i];
	}
	delete[]list;
}

void NotificationManager::resize() {
	capacity *= 2;
	Notification** newList = new Notification * [capacity];
	for (int i = 0;i< count;i++) {
		newList[i] = list[i];
	}
	delete[]list;
	list = newList;
}

void NotificationManager::addNotification(Notification* notif) {
	if (notif == nullptr)
		return;
	if (count == capacity) {
		resize();
	}
	list[count++] = notif;
}

Notification** NotificationManager::getForUser(int userId, int& outCount)const {
	outCount = 0;
	for (int i = 0;i < count;i++) {
		if (list[i]->getTargetUserId() == userId) {
			outCount = outCount + 1;
		}
	}
	if (outCount == 0)
		return nullptr;
	Notification** result = new Notification * [outCount];
	int idx = 0;
	for (int i = 0;i < count;i++) {
		if (list[i]->getTargetUserId() == userId) {
			result[idx++] = list[i];
		}
	}
	return result;
}

int NotificationManager::getUnreadCount(int userId)const {
	int unread = 0;
	for (int i = 0;i < count;i++) {
		if (list[i]->getTargetUserId() == userId && !list[i]->getIsRead()) {
			unread++;
		}
	}
	return unread;
}

void NotificationManager::markAllRead(int userId) {
	for (int i = 0;i < count;i++) {
		if (list[i]->getTargetUserId() == userId) {
			list[i]->markAsRead();
		}
	}
}


void NotificationManager::clearForUser(int userId) {
	int writeIdx = 0;
	for (int i = 0;i < count;i++) {
		if (list[i]->getTargetUserId() == userId) {
			delete list[i];

		}
		else {
			list[writeIdx++] = list[i];
		}
	}
	count = writeIdx;
}