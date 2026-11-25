#include "SharedPostMessage.h"
SharedPostMessage::SharedPostMessage(int id,int senderId,int receiverId,const std::string& timestamp,int sharedPostId):Message(id,senderId,receiverId,timestamp),sharedPostId(sharedPostId){}

int SharedPostMessage::getSharedPostId()const {
	return sharedPostId;
}

std::string SharedPostMessage::getPreview()const {
	return "[Shared Post]";
}

std::string SharedPostMessage::getType()const {
	return "SHARED_POST";

}




