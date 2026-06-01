#pragma once
#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "process_monitor.h"
#include <string>

class Display {
public:
    void render(const CPUStats& cpu, const MemoryStats& mem,
                const std::vector<ProcessInfo>& procs);
    void exportJSON(const CPUStats& cpu, const MemoryStats& mem,
                    const std::vector<ProcessInfo>& procs);
private:
    std::string progressBar(double percent, int width = 20);
    std::string currentTime();
};
