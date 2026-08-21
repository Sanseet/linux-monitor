#pragma once
#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    std::string state;
    long long mem_kb;
    double mem_percent;
    int threads;
    double cpu_percent;
};

class ProcessMonitor {
public:
    std::vector<ProcessInfo> getTopProcesses(int n = 10);
    bool getProcessByPID(int pid, ProcessInfo& process);
private:
    double getTotalMemKB();
};
