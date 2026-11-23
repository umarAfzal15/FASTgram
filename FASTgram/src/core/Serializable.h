#pragma once
#include <iostream>

class Serializable
{
public:
	virtual ~Serializable() = default; 
	virtual void write(std::ostream& out) const = 0;
	virtual void read(std::istream& in) = 0;

};