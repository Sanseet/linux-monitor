#include "cpu_monitor.h"
#include <fstream>
#include <sstream>

CPUStats CPUMonitor::readRaw() {
    CPUStats s = {};
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream ss(line);
    std::string cpu_label;
    ss >> cpu_label >> s.user >> s.nice >> s.system >> s.idle
       >> s.iowait >> s.irq >> s.softirq;
    return s;
}

CPUStats CPUMonitor::getStats() {
    CPUStats curr = readRaw();
    if (first_read_) {
        first_read_ = false;
        prev_ = curr;
        curr.usage_percent = 0.0;
        return curr;
    }
    long long prev_idle = prev_.idle + prev_.iowait;
    long long curr_idle = curr.idle + curr.iowait;
    long long prev_total = prev_.user + prev_.nice + prev_.system
                         + prev_.idle + prev_.iowait + prev_.irq + prev_.softirq;
    long long curr_total = curr.user + curr.nice + curr.system
                         + curr.idle + curr.iowait + curr.irq + curr.softirq;
    long long total_diff = curr_total - prev_total;
    long long idle_diff  = curr_idle  - prev_idle;
    curr.usage_percent = (total_diff == 0) ? 0.0 :
        (1.0 - (double)idle_diff / total_diff) * 100.0;
    prev_ = curr;
    return curr;
}
