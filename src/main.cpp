#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "process_monitor.h"
#include "display.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <unistd.h>

std::atomic<bool> export_requested(false);

void inputListener() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (true) {
        char c = getchar();
        if (c == 'e' || c == 'E') export_requested = true;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main() {
    CPUMonitor    cpu_mon;
    MemoryMonitor mem_mon;
    ProcessMonitor proc_mon;
    Display display;

    std::thread input_thread(inputListener);
    input_thread.detach();

    // warm up CPU reading
    cpu_mon.getStats();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    while (true) {
        auto cpu   = cpu_mon.getStats();
        auto mem   = mem_mon.getStats();
        auto procs = proc_mon.getTopProcesses(10);

        display.render(cpu, mem, procs);

        if (export_requested) {
            display.exportJSON(cpu, mem, procs);
            export_requested = false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
