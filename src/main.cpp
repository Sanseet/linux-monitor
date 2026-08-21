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
#include <cstdlib>
#include <string>

// Shared data
CPUStats g_cpu;
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

    mon.getStats();

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)
    );

    while (g_running) {
        auto stats = mon.getStats();

        {
            std::lock_guard<std::mutex> lock(g_cpu_mutex);
            g_cpu = stats;
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
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

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
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

        std::this_thread::sleep_for(
            std::chrono::seconds(2)
        );
    }
}

// Thread 4: Logging
void logThread() {
    while (g_running) {

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );

        double cpu;
        double mem;

        {
            std::lock_guard<std::mutex> lock(g_cpu_mutex);
            cpu = g_cpu.usage_percent;
        }

        {
            std::lock_guard<std::mutex> lock(g_mem_mutex);
            mem = g_mem.usage_percent;
        }

        std::ostringstream ss;

        ss << std::fixed
           << std::setprecision(1)
           << "CPU " << cpu << "% MEM "
           << mem << "%";

        Logger::instance().log(ss.str());
    }
}

// Input listener
void inputThread() {

    termios oldt;
    termios newt;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &newt
    );

    while (g_running) {

        char c = getchar();

        if (c == 'e' || c == 'E') {
            g_export = true;
        }

        if (c == 'q' || c == 'Q') {
            g_running = false;
        }
    }

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &oldt
    );
}

// Display one process
void displayProcess(const ProcessInfo& p) {

    std::cout << "\n";
    std::cout << "==============================\n";
    std::cout << "       Process Inspection\n";
    std::cout << "==============================\n";

    std::cout << "PID:       " << p.pid << "\n";
    std::cout << "Name:      " << p.name << "\n";
    std::cout << "State:     " << p.state << "\n";
    std::cout << "Memory:    "
              << std::fixed
              << std::setprecision(2)
              << p.mem_kb / 1024.0
              << " MB\n";

    std::cout << "Memory %:  "
              << p.mem_percent
              << "%\n";

    std::cout << "Threads:   "
              << p.threads
              << "\n";

    std::cout << "==============================\n";
}

int main(int argc, char* argv[]) {

    // -------------------------------------------------
    // PID inspection mode
    // -------------------------------------------------

    if (argc == 3 &&
        std::string(argv[1]) == "--pid") {

        int pid = std::atoi(argv[2]);

        if (pid <= 0) {
            std::cerr << "Invalid PID.\n";
            return 1;
        }

        ProcessMonitor monitor;
        ProcessInfo process;

        if (!monitor.getProcessByPID(pid, process)) {

            std::cerr
                << "Process with PID "
                << pid
                << " was not found.\n";

            return 1;
        }

        displayProcess(process);

        return 0;
    }

    // -------------------------------------------------
    // Normal monitoring mode
    // -------------------------------------------------

    Display display;

    std::thread t1(cpuThread);
    std::thread t2(memThread);
    std::thread t3(procThread);
    std::thread t4(logThread);
    std::thread t5(inputThread);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(800)
    );

    while (g_running) {

        CPUStats cpu;
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

        display.render(
            cpu,
            mem,
            procs
        );

        if (g_export) {

            display.exportJSON(
                cpu,
                mem,
                procs
            );

            g_export = false;
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }

    // Graceful thread shutdown
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}
