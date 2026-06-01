#include "memory_monitor.h"
#include <fstream>
#include <string>
#include <sstream>

MemoryStats MemoryMonitor::getStats() {
    MemoryStats s = {};
    std::ifstream file("/proc/meminfo");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key;
        long long value;
        ss >> key >> value;
        if      (key == "MemTotal:")     s.total_kb      = value;
        else if (key == "MemAvailable:") s.available_kb  = value;
        else if (key == "SwapTotal:")    s.swap_total_kb = value;
        else if (key == "SwapFree:")     s.swap_free_kb  = value;
    }
    s.used_kb      = s.total_kb - s.available_kb;
    s.swap_used_kb = s.swap_total_kb - s.swap_free_kb;
    s.usage_percent = (s.total_kb > 0) ?
        (double)s.used_kb / s.total_kb * 100.0 : 0.0;
    s.swap_percent  = (s.swap_total_kb > 0) ?
        (double)s.swap_used_kb / s.swap_total_kb * 100.0 : 0.0;
    return s;
}
