#pragma once
#include<string>
class Message  {
protected:
	int id;
	int senderId;
	int receiverId;
	std::string timestamp;
	std::string status;

public:
	Message(int id, int senderId, int receiverId, const std::string& timestamp);
	virtual ~Message() {}

	int getId()const;
	int getSenderId()const;
	int getReceiverId()const;
	std::string getTimestamp()const;
	std::string getStatus()const;

	void markDelivered();
	void markSeen();

	virtual std::string getPreview()const = 0;
	virtual std::string getType()const = 0;
};
