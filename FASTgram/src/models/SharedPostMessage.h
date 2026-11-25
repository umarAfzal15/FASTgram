#pragma once
#include "Message.h"

class SharedPostMessage : public Message {
private:
	int sharedPostId;

public:

	SharedPostMessage(int id, int senderId, int receiverId, const std::string& timestamp, int sharedPostId);
	int getSharedPostId()const;
	std::string getPreview()const override;
	std::string getType()const override;

};