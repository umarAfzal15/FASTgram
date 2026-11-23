#pragma once
#include <string>
#include <iostream>
#include "Serializable.h"

class RollNumber : public Serializable
{
private:
	int batch;
	char campus;
	int number;

	static bool isValid(int b, char c, int n);

public:
	RollNumber();
	RollNumber(int batch, char campus, int number);
	RollNumber(const std::string& text);

	int getBatch() const;
	char getCampus() const;
	int getNumber()  const;

	std::string toString() const;
	bool operator == (const RollNumber& other) const;
	bool operator != (const RollNumber& other) const;

	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};
