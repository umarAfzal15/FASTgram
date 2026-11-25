#include"PhotoMessage.h"
PhotoMessage::PhotoMessage(int id,int senderId,int receiverId,const std::string&timestamp,const std::string&imagePath):Message(id,senderId,receiverId,timestamp),imagePath(imagePath){}

std::string PhotoMessage::getImagePath()const {
	return imagePath;  //returns stored file path of img
}

std::string PhotoMessage::getPreview()const {
	return "[Photo]"; //overrides pure virtual fn from msg
}

std::string PhotoMessage::getType()const {
	return "PHOTO";//overrides pure virtual fn  getType() from msg and returns photo so UI knows hwo to render img widget instead of plain text}

}