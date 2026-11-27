#include "PollPost.h"
#include "core/BinaryIO.h"
#include <algorithm>
#include <stdexcept>

PollPost::PollPost() : Post()
{}

PollPost::PollPost(int authorId, const std::string& caption, const std::string& question, const std::vector<std::string>& options) : Post(authorId, caption)
{
	if (question.empty())
	{
		throw std::invalid_argument("PollPost question cannot be empty");
	}
	if (options.size() < MIN_OPTIONS || options.size() > MAX_OPTIONS)
	{
		throw std::invalid_argument("PollPost must have between 2 and 4 options");
	}
	for (const std::string& opt : options)
	{
		if (opt.empty())
		{
			throw std::invalid_argument("PollPost option text cannot be empty");
		}
	}

	this->question = question;
	this->options = options;
	votersPerOption.assign(options.size(), std::vector<int>());
}

const std::string& PollPost::getQuestion() const
{
	return question;
}

std::size_t PollPost::getOptionCount() const
{
	return options.size();
}

const std::string& PollPost::getOption(std::size_t index) const
{
	if (index >= options.size())
	{
		throw std::out_of_range("PollPost option index out of range");
	}
	return options[index];
}

void PollPost::vote(int userId, std::size_t optionIndex)
{
	if (userId < 1)
	{
		throw std::invalid_argument("PollPost voter userId must be at least 1");
	}
	if (optionIndex >= options.size())
	{
		throw std::out_of_range("PollPost option index out of range");
	}
	if (hasVoted(userId))
	{
		throw std::invalid_argument("User has already voted in this poll");
	}

	votersPerOption[optionIndex].push_back(userId);
}

bool PollPost::hasVoted(int userId) const
{
	for (const std::vector<int>& voters : votersPerOption)
	{
		if (std::find(voters.begin(), voters.end(), userId) != voters.end())
		{
			return true;
		}
	}
	return false;
}

int PollPost::getVoteCount(std::size_t optionIndex) const
{
	if (optionIndex >= votersPerOption.size())
	{
		throw std::out_of_range("PollPost option index out of range");
	}
	return static_cast<int>(votersPerOption[optionIndex].size());
}

int PollPost::getTotalVotes() const
{
	int total = 0;
	for (const std::vector<int>& voters : votersPerOption)
	{
		total += static_cast<int>(voters.size());
	}
	return total;
}

PostType PollPost::getPostType() const
{
	return PostType::Poll;
}

void PollPost::write(std::ostream& out) const
{
	Post::write(out);
	BinaryIO::writeString(out, question);
	BinaryIO::writeStringList(out, options);
	for (const std::vector<int>& voters : votersPerOption)
	{
		BinaryIO::writeIntList(out, voters);
	}
}

void PollPost::read(std::istream& in)
{
	Post::read(in);
	std::string readQuestion = BinaryIO::readString(in);
	std::vector<std::string> readOptions = BinaryIO::readStringList(in);

	if (readQuestion.empty() || readOptions.size() < MIN_OPTIONS || readOptions.size() > MAX_OPTIONS)
	{
		throw std::runtime_error("Corrupted PollPost data in stream");
	}

	std::vector<std::vector<int>> readVoters;
	readVoters.reserve(readOptions.size());
	for (std::size_t i = 0; i < readOptions.size(); ++i)
	{
		readVoters.push_back(BinaryIO::readIntList(in));
	}

	question = readQuestion;
	options = readOptions;
	votersPerOption = readVoters;
}