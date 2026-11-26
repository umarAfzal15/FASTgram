#include "PracticeBot.h"
#include "TextMessage.h"

PracticeBot::PracticeBot()
    : Account("studybot", "FAST Study Buddy", "bot1234"),
    replyPending(false),
    replyTimer(0.0f),
    pendingUserId(-1),
    pendingReplyText("") {
    setBio("Official FASTagram Practice Partner! Send a message to test live replies.");
    setPrivate(false);
}

AccountType PracticeBot::getType() const {
    return AccountType::Society;
}

std::string PracticeBot::getBadgeLabel() const {
    return "Study Bot";
}

bool PracticeBot::canPostNotice() const {
    return false;
}

bool PracticeBot::canDeleteAnyPost() const {
    return false;
}

void PracticeBot::scheduleReply(int userId, const std::string& incomingText) {
    pendingUserId = userId;
    replyPending = true;
    replyTimer = 1.5f;

    if (incomingText.find("exam") != std::string::npos || incomingText.find("study") != std::string::npos) {
        pendingReplyText = "Good luck with finals! Let's crush OOP together.";
    }
    else if (incomingText.find("hi") != std::string::npos || incomingText.find("hello") != std::string::npos) {
        pendingReplyText = "Hey there! Need a study partner on FASTagram?";
    }
    else {
        pendingReplyText = "Got your message: \"" + incomingText + "\" - keep going!";
    }
}

void PracticeBot::update(float dt, ChatManager& chatManager, const SocialGraph& graph) {
    if (!replyPending) return;

    replyTimer -= dt;
    if (replyTimer <= 0.0f) {
        replyPending = false;
        replyTimer = 0.0f;

        chatManager.sendTextMessage(getId(), pendingUserId, "Just now", pendingReplyText);
    }
}