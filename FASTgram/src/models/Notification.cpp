#include"Notification.h"
using namespace std;

Notification::Notification(){
	id = 0;
	targetUserId = 0;
	actorUserId = 0;
	text = "";
	isRead = false;
	timestamp = "";


}

Notification::Notification(int id,int targetUserId,int actorUserId,const string&text,const string& timestamp) {
	this->id = id;
	this->targetUserId = targetUserId;
	this->actorUserId = actorUserId;
	this->text = text;
	this->isRead = false;
	this->timestamp = timestamp;

}

int Notification::getId()const {
	return id;
	}
int Notification::getTargetUserId()const {
	return targetUserId;
}

int Notification::getActorUserId()const {
	return actorUserId;
}

string Notification::getText()const {
	return text;
}

bool Notification::getIsRead()const {
	return isRead;
}

string Notification::getTimestamp()const {
	return timestamp;
}

void Notification::markAsRead(){
	isRead = true;
}