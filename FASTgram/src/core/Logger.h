#pragma once
#include <string>
#include <fstream>
#include <ctime>

class Logger {
public:
    static void log(const std::string& action) {
        std::ofstream file("data/activity.log", std::ios::app);
        if (!file.is_open()) return;

        std::time_t now = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &now);

        char timeStr[20];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &localTime);

        file << "[" << timeStr << "] " << action << "\n";
    }
};

inline void logActivity(const std::string& action) {
    Logger::log(action);
}