#include "process_monitor.h"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <algorithm>
#include <cctype>

double ProcessMonitor::getTotalMemKB() {
    std::ifstream f("/proc/meminfo");
    std::string key;
    long long val;

    f >> key >> val;

    return (double)val;
}

std::vector<ProcessInfo> ProcessMonitor::getTopProcesses(int n) {
    std::vector<ProcessInfo> procs;

    double total_mem = getTotalMemKB();

    DIR* dir = opendir("/proc");

    if (!dir) {
        return procs;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);

        if (!std::all_of(name.begin(), name.end(), ::isdigit)) {
            continue;
        }

        int pid = std::stoi(name);

        std::ifstream status("/proc/" + name + "/status");

        if (!status.is_open()) {
            continue;
        }

        ProcessInfo p;

        p.pid = pid;
        p.cpu_percent = 0.0;
        p.mem_kb = 0;
        p.mem_percent = 0.0;
        p.threads = 0;

        std::string line;

        while (std::getline(status, line)) {
            std::istringstream ss(line);
            std::string key;

            ss >> key;

            if (key == "Name:") {
                ss >> p.name;
            }
            else if (key == "State:") {
                ss >> p.state;
            }
            else if (key == "Threads:") {
                ss >> p.threads;
            }
            else if (key == "VmRSS:") {
                ss >> p.mem_kb;
            }
        }

        p.mem_percent =
            (total_mem > 0)
            ? (double)p.mem_kb / total_mem * 100.0
            : 0.0;

        procs.push_back(p);
    }

    closedir(dir);

    std::sort(
        procs.begin(),
        procs.end(),
        [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.mem_kb > b.mem_kb;
        }
    );

    if ((int)procs.size() > n) {
        procs.resize(n);
    }

    return procs;
}

bool ProcessMonitor::getProcessByPID(int pid, ProcessInfo& p) {

    double total_mem = getTotalMemKB();

    std::string pid_str = std::to_string(pid);

    std::ifstream status(
        "/proc/" + pid_str + "/status"
    );

    if (!status.is_open()) {
        return false;
    }

    p.pid = pid;
    p.cpu_percent = 0.0;
    p.mem_kb = 0;
    p.mem_percent = 0.0;
    p.threads = 0;

    std::string line;

    while (std::getline(status, line)) {

        std::istringstream ss(line);

        std::string key;

        ss >> key;

        if (key == "Name:") {
            ss >> p.name;
        }
        else if (key == "State:") {
            ss >> p.state;
        }
        else if (key == "Threads:") {
            ss >> p.threads;
        }
        else if (key == "VmRSS:") {
            ss >> p.mem_kb;
        }
    }

    p.mem_percent =
        (total_mem > 0)
        ? (double)p.mem_kb / total_mem * 100.0
        : 0.0;

    return true;
}
