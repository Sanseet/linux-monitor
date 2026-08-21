#include "process_monitor.h"

#include <gtest/gtest.h>
#include <unistd.h>

TEST(ProcessMonitorTest, FindsCurrentProcess) {
    ProcessMonitor monitor;

    ProcessInfo process;

    int pid = getpid();

    EXPECT_TRUE(
        monitor.getProcessByPID(pid, process)
    );

    EXPECT_EQ(process.pid, pid);
    EXPECT_FALSE(process.name.empty());
    EXPECT_GE(process.threads, 1);
}

TEST(ProcessMonitorTest, RejectsInvalidPID) {
    ProcessMonitor monitor;

    ProcessInfo process;

    EXPECT_FALSE(
        monitor.getProcessByPID(999999, process)
    );
}

TEST(ProcessMonitorTest, ReadsMemoryInformation) {
    ProcessMonitor monitor;

    ProcessInfo process;

    int pid = getpid();

    ASSERT_TRUE(
        monitor.getProcessByPID(pid, process)
    );

    EXPECT_GE(process.mem_kb, 0);
    EXPECT_GE(process.mem_percent, 0.0);
}

TEST(ProcessMonitorTest, ReadsProcessState) {
    ProcessMonitor monitor;

    ProcessInfo process;

    int pid = getpid();

    ASSERT_TRUE(
        monitor.getProcessByPID(pid, process)
    );

    EXPECT_FALSE(process.state.empty());
}
