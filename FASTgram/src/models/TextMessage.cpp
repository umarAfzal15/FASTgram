#include "TextMessage.h"
TextMessage::TextMessage(int id, int senderId, int receiverId, const std::string& timestamp, const std::string& text)
    : Message(id, senderId, receiverId, timestamp), text(text) {
}

    std::string TextMessage::getText()const {
        return text;
    }

    std::string TextMessage::getPreview()const {
        return text;
    }

    std::string TextMessage::getType()const {
        return "TEXT";

    }