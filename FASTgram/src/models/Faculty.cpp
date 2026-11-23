#include "Faculty.h"

Faculty::Faculty() : Account() {
}

Faculty::Faculty(const std::string& username, const std::string& displayName, const std::string& password)
    : Account(username, displayName, password) {
}

AccountType Faculty::getType() const {
    return AccountType::Faculty; // Match the enum name in Account.h
}

std::string Faculty::getBadgeLabel() const {
    return "Faculty";
}

bool Faculty::canPostNotice() const {
    return true;
}

bool Faculty::canDeleteAnyPost() const {
    return false;
}