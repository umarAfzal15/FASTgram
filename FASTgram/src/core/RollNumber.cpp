#include "RollNumber.h"
#include <cctype>  // for isalpha(), isdigit(), toupper()
#include <iomanip> 
#include <sstream> // to store a string in memory and then display it
#include <stdexcept> // for showing the built in errors

bool RollNumber::isValid(int b, char c, int n)
{
	if (b < 0 || b > 99)
	{
		return false;
	}

	if (!std::isalpha(static_cast<unsigned char>(c)))
	{
		return false;
	}

	if (n < 0 || n > 9999)
	{
		return false;
	}

	return true;
		
}

RollNumber::RollNumber() : batch(0), campus('\0'), number(0)
{
}

RollNumber::RollNumber(int batch, char campus, int number) : batch(0), campus('\0'), number(0)
{
	char upperCampus = static_cast<char>(std::toupper(static_cast<unsigned char>(campus)));
	if (!isValid(batch, upperCampus, number))
	{
		throw std::invalid_argument("Invalid roll number values");
	}
	
	this->batch = batch;
	this->campus = upperCampus;
	this->number = number;
}

RollNumber::RollNumber(const std::string& text) : batch(0), campus('\0'), number(0)
{
	if (text.length() != 8 || text[3] != '-')
	{
		throw std::invalid_argument("Roll number must follow the format XXC-NNNN");
	}

	if (!std::isdigit(static_cast<unsigned char>(text[0])) ||
		!std::isdigit(static_cast<unsigned char>(text[1])) ||
		!std::isalpha(static_cast<unsigned char>(text[2])))
	{
		throw std::invalid_argument("Invalid batch or campus in roll number");
	}

	for (int i = 4; i < 8; ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(text[i])))
		{
			throw std::invalid_argument("Invalid serial digits in roll number");
		}
	}

	int parsedBatch = std::stoi(text.substr(0, 2));
	char parsedCampus = static_cast<char>(std::toupper(static_cast<unsigned char>(text[2])));
	int parsedNumber = std::stoi(text.substr(4, 4));

	if (!isValid(parsedBatch, parsedCampus, parsedNumber))
	{
		throw std::invalid_argument("Roll number out of valid range");
	}

	this->batch = parsedBatch;
	this->campus = parsedCampus;
	this->number = parsedNumber;

}

int RollNumber::getBatch() const
{
	return batch;
}

char RollNumber::getCampus() const
{
	return campus;
}

int RollNumber::getNumber() const
{
	return number;
}

std::string RollNumber::toString() const
{
	std::ostringstream out;
	out << std::setw(2) << std::setfill('0') << batch << campus << "-"
		<< std::setw(4) << std::setfill('0') << number;

	return out.str();
}

bool RollNumber::operator==(const RollNumber& other) const
{
	return batch == other.batch && campus == other.campus && number == other.number;
}

bool RollNumber::operator!=(const RollNumber& other) const
{
	return !(*this == other);
}

void RollNumber::write(std::ostream& out) const
{
	out.write(reinterpret_cast<const char*>(&batch), sizeof(batch));
	out.write(reinterpret_cast<const char*>(&campus), sizeof(campus));
	out.write(reinterpret_cast<const char*>(&number), sizeof(number));
}

void RollNumber::read(std::istream& in)
{
	int readBatch = 0;
	char readCampus = '\0';
	int readNumber = 0;

	in.read(reinterpret_cast<char*>(&readBatch), sizeof(readBatch));
	in.read(reinterpret_cast<char*>(&readCampus), sizeof(readCampus));
	in.read(reinterpret_cast<char*>(&readNumber), sizeof(readNumber));

	if (!in)
	{
		throw std::runtime_error("Failed to read RollNumber from stream");
	}

	char upperCampus = static_cast<char>(std::toupper(static_cast<unsigned char>(readCampus)));
	
	if (!isValid(readBatch, upperCampus, readNumber))
	{
		throw std::invalid_argument("Corrupted RollNumber data in stream");
	}

	batch = readBatch;
	campus = upperCampus;
	number = readNumber;
}