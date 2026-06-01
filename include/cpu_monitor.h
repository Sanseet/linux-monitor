#pragma once
#include <string>

struct CPUStats {
    double usage_percent;
    long long user, nice, system, idle, iowait, irq, softirq;
};

class CPUMonitor {
public:
    CPUStats getStats();
private:
    CPUStats prev_ = {};
    bool first_read_ = true;
    CPUStats readRaw();
};
