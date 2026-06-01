#include "display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

std::string Display::progressBar(double percent, int width) {
    int filled = (int)(percent / 100.0 * width);
    if (filled > width) filled = width;
    std::string bar = "[";
    for (int i = 0; i < width; i++)
        bar += (i < filled) ? "=" : " ";
    bar += "]";
    return bar;
}

std::string Display::currentTime() {
    time_t now = time(nullptr);
    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    return std::string(buf);
}

void Display::render(const CPUStats& cpu, const MemoryStats& mem,
                     const std::vector<ProcessInfo>& procs) {
    std::cout << "\033[2J\033[H";
    std::cout << "==============================\n";
    std::cout << "   Linux System Monitor\n";
    std::cout << "   Time: " << currentTime() << "\n";
    std::cout << "==============================\n\n";

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "CPU:    " << progressBar(cpu.usage_percent)
              << " " << cpu.usage_percent << "%\n";

    double mem_gb      = mem.used_kb      / 1024.0 / 1024.0;
    double mem_total   = mem.total_kb     / 1024.0 / 1024.0;
    double swap_gb     = mem.swap_used_kb / 1024.0 / 1024.0;
    double swap_total  = mem.swap_total_kb/ 1024.0 / 1024.0;

    std::cout << "Memory: " << progressBar(mem.usage_percent)
              << " " << mem_gb << " GB / " << mem_total << " GB"
              << " (" << mem.usage_percent << "%)\n";
    std::cout << "Swap:   " << progressBar(mem.swap_percent)
              << " " << swap_gb << " GB / " << swap_total << " GB"
              << " (" << mem.swap_percent << "%)\n\n";

    std::cout << std::left
              << std::setw(8)  << "PID"
              << std::setw(20) << "NAME"
              << std::setw(8)  << "MEM%"
              << std::setw(10) << "MEM(MB)"
              << std::setw(10) << "THREADS"
              << std::setw(6)  << "STATE" << "\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& p : procs) {
        std::cout << std::setw(8)  << p.pid
                  << std::setw(20) << p.name
                  << std::setw(8)  << std::setprecision(1) << p.mem_percent
                  << std::setw(10) << std::setprecision(1) << p.mem_kb/1024.0
                  << std::setw(10) << p.threads
                  << std::setw(6)  << p.state << "\n";
    }
    std::cout << "\n[Ctrl+C to exit]  [e + Enter to export JSON]\n";
}

void Display::exportJSON(const CPUStats& cpu, const MemoryStats& mem,
                         const std::vector<ProcessInfo>& procs) {
    // Always write to project root data/logs/
    mkdir("/home/sanse/linux-monitor/data", 0755);
    mkdir("/home/sanse/linux-monitor/data/logs", 0755);

    time_t now = time(nullptr);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", localtime(&now));
    std::string filename = "/home/sanse/linux-monitor/data/logs/snapshot_"
                         + std::string(timebuf) + ".json";
    std::ofstream f(filename);
    if (!f.is_open()) {
        std::cout << "ERROR: Could not open " << filename << "\n";
        return;
    }
    f << "{\n";
    f << "  \"timestamp\": \"" << timebuf << "\",\n";
    f << "  \"cpu_percent\": " << cpu.usage_percent << ",\n";
    f << "  \"memory\": {\n";
    f << "    \"used_kb\": " << mem.used_kb << ",\n";
    f << "    \"total_kb\": " << mem.total_kb << ",\n";
    f << "    \"percent\": " << mem.usage_percent << "\n  },\n";
    f << "  \"processes\": [\n";
    for (size_t i = 0; i < procs.size(); i++) {
        f << "    {\"pid\": " << procs[i].pid
          << ", \"name\": \"" << procs[i].name << "\""
          << ", \"mem_kb\": " << procs[i].mem_kb
          << ", \"threads\": " << procs[i].threads << "}";
        if (i + 1 < procs.size()) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
    std::cout << "\nExported to " << filename << "\n";
}
