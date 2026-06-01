#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& instance();
    void log(const std::string& message);
private:
    Logger();
    std::ofstream file_;
    std::mutex mutex_;
};
