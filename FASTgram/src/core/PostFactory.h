#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "models/Post.h"

struct PostData
{
	int authorId = 0;
	std::string caption;
	std::string imagePath;                  
	std::vector<std::string> images;         
	std::string question;                    
	std::vector<std::string> options;        
};

class PostFactory
{
public:
	PostFactory() = delete; 

	// Both of these return a new object and the Caller owns it (and then the Post Manager deletes it)
	static Post* createBlank(uint8_t tag);           
	static Post* create(PostType type, const PostData& data); 
};