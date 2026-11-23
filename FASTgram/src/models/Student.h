#pragma once
#include "Account.h"
#include "core/RollNumber.h"

class Student : public Account
{
private:
    RollNumber rollNumber;

public:
    Student();
    Student(const std::string& username, const std::string& displayName, const std::string& rawPassword, const RollNumber& roll);

    const RollNumber& getRollNumber() const;

    AccountType getType() const override;
    std::string getBadgeLabel() const override;
    bool canPostNotice() const override;
    bool canDeleteAnyPost() const override;

    void write(std::ostream& out) const override;
    void read(std::istream& in) override;
};