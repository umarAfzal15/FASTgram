#pragma once
#include <string>
#include "Account.h"
#include "../services/ChatManager.h"
#include "SocialGraph.h"

class PracticeBot : public Account {
private:
    bool replyPending;
    float replyTimer;
    int pendingUserId;
    std::string pendingReplyText;

public:
    PracticeBot();

    AccountType getType() const override;
    std::string getBadgeLabel() const override;
    bool canPostNotice() const override;
    bool canDeleteAnyPost() const override;

    void scheduleReply(int userId, const std::string& incomingText);
    void update(float dt, ChatManager& chatManager, const SocialGraph& graph);
};