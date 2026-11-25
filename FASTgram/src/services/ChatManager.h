#pragma once
#include <vector>
#include <string>
#include "../models/Message.h"
#include "../models/TextMessage.h"
#include "../models/PhotoMessage.h"
#include "../models/SharedPostMessage.h"
#include "../models/SocialGraph.h"

class ChatManager {
private:

	std::vector<Message*> messages;
	int nextMessageId;
	int botUserId;
	void triggerBotReply(int userId, const std::string& userText, const std::string& timestamp);

public:

	ChatManager(int botUserId = 999);
	~ChatManager();//destructor sath hi likhdia

	void sendTextMessage(int senderId, int receiverId, const std::string& timestamp, const std::string& text);
	void sendPhotoMessage(int senderId, int receiverId, const std::string& timestamp, const std::string& imagePath);
	void sendSharedPostMessage(int senderId, int receiverId, const std::string& timestamp, int sharedPostId);

	std::vector<Message*> getConversation(int userA, int userB) const;
	void markConversationSeen(int viewerId, int partnerId);
	int getUnreadCount(int viewerId, int partnerId) const;
	std::string getLastMessageSummary(int userA, int userB) const;

	std::vector<int> getInboxPartners(int userId, const SocialGraph& graph) const;
	std::vector<int> getMessageRequestPartners(int userId, const SocialGraph& graph) const;
};



