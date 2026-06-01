#include "logger.h"
#include <ctime>
#include <iostream>

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

Logger::Logger() {
    file_.open("/home/sanse/linux-monitor/data/logs/system.log",
               std::ios::app);
}

void Logger::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    std::string line = std::string(buf) + " " + msg;
    file_ << line << "\n";
    file_.flush();
}
