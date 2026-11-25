#include"Message.h"
Message::Message(int id,int senderId,int receiverId,const std::string&timestamp):id(id),senderId(senderId),receiverId(receiverId),timestamp(timestamp),status("Sent"){}

int Message::getId()const {
	return id;
}

int Message::getSenderId()const {
	return senderId;
}

int Message::getReceiverId()const {
	return receiverId;
}

std::string Message::getTimestamp()const {
	return timestamp;
}


std::string Message::getStatus()const {
	return status;
}

void Message::markDelivered() {
	if (status == "Sent") {
		status = "Delivered";
	}
}

void Message::markSeen() {
	status = "Seen";
}