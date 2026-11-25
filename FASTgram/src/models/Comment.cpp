#include"Comment.h"
Comment::Comment(int commentId,int authorId,const std::string&text,const std::string&timestamp):commentId(commentId),authorId(authorId),text(text),timestamp(timestamp){}

int Comment::getCommentId()const {
	return commentId;
}
int Comment::getAuthorId()const {
	return authorId;
}

std::string Comment::getText()const {
	return text;
}

std::string Comment::getTimestamp()const {
	return timestamp;
}