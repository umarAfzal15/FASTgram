#include "Time.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

Time::Time() : timestamp(0) {}

Time::Time(std::time_t rawTime) : timestamp(rawTime)
{}
Time Time::now()
{
	return Time(std::time(nullptr));
}

std::time_t Time::getRaw() const
{
	return timestamp;
}

std::string Time::toString() const
{
	std::tm timeInfo{};

#ifdef _WIN32
	localtime_s(&timeInfo, &timestamp);

#else
	localtime_r(&timestamp, &timeInfo);

#endif

	std::ostringstream oss;

	oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M");
	return oss.str();

}

bool Time::operator<(const Time& other) const
{
	return timestamp < other.timestamp;
}

bool Time::operator==(const Time& other) const
{
	return timestamp == other.timestamp;
}

bool Time::operator!=(const Time& other) const
{
	return timestamp != other.timestamp;
}

bool Time::operator>(const Time& other) const
{
	return timestamp > other.timestamp;
}

bool Time::operator<=(const Time& other) const
{
	return timestamp <= other.timestamp;
}

bool Time::operator>=(const Time& other) const
{
	return timestamp >= other.timestamp;
}

long long Time::operator-(const Time& other) const
{
	return static_cast<long long>(timestamp) - static_cast<long long>(other.timestamp);
}

void Time::write(std::ostream& out) const
{
	long long value = static_cast<long long>(timestamp);
	out.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void Time::read(std::istream& in)
{
	long long value = 0;
	in.read(reinterpret_cast<char*>(&value), sizeof(value));

	if (!in)
	{
		throw std::runtime_error("Failed to read Time from stream");
	}

	timestamp = static_cast<std::time_t>(value);
}