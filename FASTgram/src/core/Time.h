#pragma once

#include "Serializable.h"
#include <ctime>
#include <iostream>
#include <string>

class Time : public Serializable
{
private:
	std::time_t timestamp;

public:
	Time();
	explicit Time(std::time_t rawTime);
	static Time now();

	std::time_t getRaw() const;
	std::string toString() const;

	bool operator<(const Time& other) const;
	bool operator==(const Time& other) const;
	bool operator!=(const Time& other) const;
	bool operator>(const Time& other) const;
	bool operator<=(const Time& other) const;
	bool operator>=(const Time& other) const;
	long long operator-(const Time& other) const;
	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};

