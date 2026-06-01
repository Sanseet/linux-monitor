#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "process_monitor.h"
#include "display.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

// Shared data
CPUStats    g_cpu;
MemoryStats g_mem;
std::vector<ProcessInfo> g_procs;

std::mutex g_cpu_mutex;
std::mutex g_mem_mutex;
std::mutex g_proc_mutex;

std::atomic<bool> g_running(true);
std::atomic<bool> g_export(false);

// Thread 1: CPU
void cpuThread() {
    CPUMonitor mon;
    mon.getStats(); // warmup
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while (g_running) {
        auto stats = mon.getStats();
        {
            std::lock_guard<std::mutex> lock(g_cpu_mutex);
            g_cpu = stats;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Thread 2: Memory
void memThread() {
    MemoryMonitor mon;
    while (g_running) {
        auto stats = mon.getStats();
        {
            std::lock_guard<std::mutex> lock(g_mem_mutex);
            g_mem = stats;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Thread 3: Process
void procThread() {
    ProcessMonitor mon;
    while (g_running) {
        auto procs = mon.getTopProcesses(10);
        {
            std::lock_guard<std::mutex> lock(g_proc_mutex);
            g_procs = procs;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

// Thread 4: Logging
void logThread() {
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        double cpu, mem;
        {
            std::lock_guard<std::mutex> lock(g_cpu_mutex);
            cpu = g_cpu.usage_percent;
        }
        {
            std::lock_guard<std::mutex> lock(g_mem_mutex);
            mem = g_mem.usage_percent;
        }
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1)
           << "CPU " << cpu << "% MEM " << mem << "%";
        Logger::instance().log(ss.str());
    }
}

// Input listener
void inputThread() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (g_running) {
        char c = getchar();
        if (c == 'e' || c == 'E') g_export = true;
        if (c == 'q' || c == 'Q') g_running = false;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main() {
    Display display;

    std::thread t1(cpuThread);
    std::thread t2(memThread);
    std::thread t3(procThread);
    std::thread t4(logThread);
    std::thread t5(inputThread);

    t1.detach();
    t2.detach();
    t3.detach();
    t4.detach();
    t5.detach();

    // Wait for first readings
    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    while (g_running) {
        CPUStats    cpu;
        MemoryStats mem;
        std::vector<ProcessInfo> procs;

        {
            std::lock_guard<std::mutex> lock(g_cpu_mutex);
            cpu = g_cpu;
        }
        {
            std::lock_guard<std::mutex> lock(g_mem_mutex);
            mem = g_mem;
        }
        {
            std::lock_guard<std::mutex> lock(g_proc_mutex);
            procs = g_procs;
        }

        display.render(cpu, mem, procs);

        if (g_export) {
            display.exportJSON(cpu, mem, procs);
            g_export = false;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
