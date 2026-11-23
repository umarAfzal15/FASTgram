#include "Student.h"

Student::Student() : Account()
{}

Student::Student(const std::string& username, const std::string& displayName, const std::string& rawPassword, const RollNumber& roll) : Account(username, displayName, rawPassword), rollNumber(roll)
{}

const RollNumber& Student::getRollNumber() const
{
    return rollNumber;
}

AccountType Student::getType() const
{
    return AccountType::Student;
}

std::string Student::getBadgeLabel() const
{
    return "Student";
}

bool Student::canPostNotice() const
{
    return false;
}

bool Student::canDeleteAnyPost() const
{
    return false;
}

void Student::write(std::ostream& out) const
{
    Account::write(out);
    rollNumber.write(out);
}

void Student::read(std::istream& in)
{
    Account::read(in);
    rollNumber.read(in);
}