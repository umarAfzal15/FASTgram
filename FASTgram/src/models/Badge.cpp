#include"Badge.h"
using namespace std;

Badge::Badge(const string& name, const string& description) {
	this->name = name;
	this->description = description;
}
string Badge::getName()const {
	return name;
}
string Badge::getDescription()const {
	return description;
}

FirstPostBadge::FirstPostBadge() :Badge("First Post", "Published your first post on Fastagram!") { //parent consttructor called 

}

bool FirstPostBadge::isEarned(int postCount, int streakDays)const {
	if (postCount >= 1)
		return true; //if hte user has at least one post unlock
	else
		return false;
}

StreakBadge::StreakBadge(int requiredDays):Badge("StreakMaster","Maintained a daily pisting streak!"){
	this->requiredDays = requiredDays;

}//init. streakbadge with custom day requirement

bool StreakBadge::isEarned(int postCount, int streakDays)const {
	if (streakDays >= this->requiredDays)
		return true;
	else
		return false;

}