#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "Post.h"

class PollPost : public Post
{
private:
	std::string question;
	std::vector<std::string> options;
	std::vector<std::vector<int>> votersPerOption;

	static const std::size_t MIN_OPTIONS = 2;
	static const std::size_t MAX_OPTIONS = 4;

public:
	PollPost(); 
	PollPost(int authorId, const std::string& caption, const std::string& question, const std::vector<std::string>& options);

	const std::string& getQuestion() const;
	std::size_t getOptionCount() const;
	const std::string& getOption(std::size_t index) const;

	void vote(int userId, std::size_t optionIndex); 
	bool hasVoted(int userId) const;
	int getVoteCount(std::size_t optionIndex) const;
	int getTotalVotes() const;

	PostType getPostType() const override;

	void write(std::ostream& out) const override;
	void read(std::istream& in) override;
};