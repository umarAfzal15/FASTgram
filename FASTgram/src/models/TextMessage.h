#pragma once
#include"Message.h"
class TextMessage :public Message {
private:
	std::string text;

public:
	TextMessage(int id, int senderId, int receiverId, const std::string& timestamp, const std::string& text);
	std::string getText()const;
	std::string getPreview()const override;
	std::string getType()const override;
};