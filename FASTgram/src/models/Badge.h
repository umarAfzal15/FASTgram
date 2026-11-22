#pragma once
#include<string>
class Badge {
protected:
	std::string name;
	std::string description;
public:
	Badge(const std::string& name, const std::string& description);
		virtual ~Badge() = default;
		std::string getName()const;


			std::string getDescription()const;
		
			virtual bool isEarned(int postCount, int streakDays)const = 0;
};

class FirstPostBadge :public Badge {
public:
	FirstPostBadge();

	bool isEarned(int postCount, int streakDays)const override;
};


	class StreakBadge :public Badge {
	private:
		int requiredDays;


	public:
		StreakBadge(int requiredDays = 7);

		bool isEarned(int postcount, int streakDays)const override;

	};


