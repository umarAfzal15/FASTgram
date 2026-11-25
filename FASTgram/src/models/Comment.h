#pragma once
#include<string>
#include"Likeable.h"

class Comment :public Likeable {
private:
	int commentId;
	int authorId;
	std::string text;
	std::string timestamp;

public:

	Comment(int commentId, int authorId, const std::string& text, const std::string& timestamp = "");
	
	//implmenting geTter fns below:

	int getCommentId()const;
	int getAuthorId()const;
	std::string getText()const; 
	std::string getTimestamp()const;

};