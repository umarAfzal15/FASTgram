#include"ChatManager.h"
ChatManager::ChatManager(int botUserId):nextMessageId(1),botUserId(botUserId){}

ChatManager::~ChatManager() {
    for (int i = 0; i < (int)messages.size(); i++) {
        delete messages[i];
    }
    messages.clear();
}

void ChatManager::sendTextMessage(int senderId, int receiverId, const std::string& timestamp, const std::string& text) {
    Message* msg = new TextMessage(nextMessageId++, senderId, receiverId, timestamp, text);
    messages.push_back(msg);

    // Feature M6: Practice-partner bot auto-reply
    if (receiverId == botUserId && senderId != botUserId) {
        triggerBotReply(senderId, text, timestamp);
    }
}

void ChatManager::sendPhotoMessage(int senderId, int receiverId, const std::string& timestamp, const std::string& imagePath) {
    Message* msg = new PhotoMessage(nextMessageId++, senderId, receiverId, timestamp, imagePath);
    messages.push_back(msg);

    if (receiverId == botUserId && senderId != botUserId) {
        triggerBotReply(senderId, "[Photo]", timestamp);
    }
}

void ChatManager::sendSharedPostMessage(int senderId, int receiverId, const std::string& timestamp, int sharedPostId) {
    Message* msg = new SharedPostMessage(nextMessageId++, senderId, receiverId, timestamp, sharedPostId);
    messages.push_back(msg);

    if (receiverId == botUserId && senderId != botUserId) {
        triggerBotReply(senderId, "[Shared Post]", timestamp);
    }
}

void ChatManager::triggerBotReply(int userId, const std::string& userText, const std::string& timestamp) {
    std::string reply = "Bot reply: Got your message (" + userText + ")!";
    Message* botMsg = new TextMessage(nextMessageId++, botUserId, userId, timestamp, reply);
    messages.push_back(botMsg);
}

std::vector<Message*> ChatManager::getConversation(int userA, int userB) const {
    std::vector<Message*> result;
    for (int i = 0; i < (int)messages.size(); i++) {
        int s = messages[i]->getSenderId();
        int r = messages[i]->getReceiverId();
        if ((s == userA && r == userB) || (s == userB && r == userA)) {
            result.push_back(messages[i]);
        }
    }
    return result;
}

void ChatManager::markConversationSeen(int viewerId, int partnerId) {
    for (int i = 0; i < (int)messages.size(); i++) {
        if (messages[i]->getSenderId() == partnerId && messages[i]->getReceiverId() == viewerId) {
            messages[i]->markSeen();
        }
    }
}

int ChatManager::getUnreadCount(int viewerId, int partnerId) const {
    int count = 0;
    for (int i = 0; i < (int)messages.size(); i++) {
        if (messages[i]->getSenderId() == partnerId &&
            messages[i]->getReceiverId() == viewerId &&
            messages[i]->getStatus() != "Seen") {
            count++;
        }
    }
    return count;
}

std::string ChatManager::getLastMessageSummary(int userA, int userB) const {
    for (int i = (int)messages.size() - 1; i >= 0; i--) {
        int s = messages[i]->getSenderId();
        int r = messages[i]->getReceiverId();
        if ((s == userA && r == userB) || (s == userB && r == userA)) {
            return messages[i]->getPreview();
        }
    }
    return "";
}

std::vector<int> ChatManager::getInboxPartners(int userId, const SocialGraph& graph) const {
    std::vector<int> partners;
    for (int i = 0; i < (int)messages.size(); i++) {
        int s = messages[i]->getSenderId();
        int r = messages[i]->getReceiverId();
        int other = -1;

        if (s == userId) other = r;
        else if (r == userId) other = s;

        if (other != -1) {
            if (graph.isFollowing(userId, other) || other == botUserId) {
                bool exists = false;
                for (int j = 0; j < (int)partners.size(); j++) {
                    if (partners[j] == other) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) partners.push_back(other);
            }
        }
    }
    return partners;
}

std::vector<int> ChatManager::getMessageRequestPartners(int userId, const SocialGraph& graph) const {
    std::vector<int> requests;
    for (int i = 0; i < (int)messages.size(); i++) {
        int s = messages[i]->getSenderId();
        int r = messages[i]->getReceiverId();

        if (r == userId && s != botUserId && !graph.isFollowing(userId, s)) {
            bool exists = false;
            for (int j = 0; j < (int)requests.size(); j++) {
                if (requests[j] == s) {
                    exists = true;
                    break;
                }
            }
            if (!exists) requests.push_back(s);
        }
    }
    return requests;
}