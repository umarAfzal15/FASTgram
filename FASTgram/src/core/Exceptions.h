#pragma once
#include<stdexcept>
#include<string>

class ValidationException : public std::runtime_error {//for avoiding issues caUsed by errors
public:
	explicit ValidationException(const std::string& message) :std::runtime_error(message) {

	}

};


class AuthException : public std::runtime_error {//for avoiding issues caUsed by errors
public:
	explicit AuthException(const std::string& message) :std::runtime_error(message) {

	}

};


class FileCorruptException : public std::runtime_error {//for avoiding issues caUsed by errors
public:
	explicit FileCorruptException(const std::string& message) :std::runtime_error(message) {

	}

};




