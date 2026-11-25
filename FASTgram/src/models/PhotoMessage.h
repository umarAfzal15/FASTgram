#pragma once
#include"Message.h"

class PhotoMessage :public Message {
private:
	std::string imagePath;

public:

	PhotoMessage(int id, int senderId, int receiverId, const std::string& timestamp, const std::string& imagePath);
	std::string getImagePath()const;

	std::string getPreview()const override;
	std::string getType()const override;
};


