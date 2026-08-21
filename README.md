# Linux System Monitor

A lightweight Linux system monitoring utility built in **C++17** using Linux `/proc` interfaces, POSIX APIs, multithreading, and CMake.

The application monitors CPU, memory, swap, processes, and threads in real time and provides command-line process inspection using a PID.

---

## Features

- Real-time CPU monitoring
- Memory monitoring
- Swap monitoring
- Process inspection
- Thread tracking
- Process state analysis
- Multithreaded metric collection
- Thread-safe shared state
- PID-level process inspection
- Invalid PID handling
- JSON export
- GoogleTest unit testing
- CTest regression testing
- GDB debugging support
- AddressSanitizer support
- UndefinedBehaviorSanitizer support

---

## Architecture

```text
                         Linux /proc
                              |
             +----------------+----------------+
             |                |                |
             v                v                v
        /proc/stat      /proc/meminfo      /proc/<pid>
             |                |                |
             v                v                v
       CPU Monitor      Memory Monitor    Process Monitor
             |                |                |
             +----------------+----------------+
                              |
                              v
                       Shared Metrics
                              |
                     Thread-Safe State
                              |
                              v
                    Terminal Dashboard
                         /         \
                        /           \
                       v             v
                   Display       JSON Export
```

---

## Multithreading

The monitoring system uses separate worker threads for different responsibilities:

```text
CPU Thread
    |
    +----> CPU statistics

Memory Thread
    |
    +----> Memory / Swap statistics

Process Thread
    |
    +----> Process information

Logging Thread
    |
    +----> Periodic logging

Input Thread
    |
    +----> User commands
```

Shared monitoring data is protected using:

- `std::thread`
- `std::mutex`
- `std::lock_guard`
- `std::atomic`

Worker threads are explicitly joined during shutdown to provide controlled thread lifecycle management.

---

## Linux `/proc` Integration

The application uses the Linux `/proc` virtual filesystem as its primary source of system information.

### CPU

```text
/proc/stat
```

Used to obtain CPU statistics and calculate CPU utilization.

### Memory

```text
/proc/meminfo
```

Used to obtain memory and swap information.

### Processes

```text
/proc/<pid>/status
```

Used to obtain process information such as:

- Process name
- Process state
- Resident memory
- Thread count

This makes the project Linux-specific and demonstrates interaction with operating-system level interfaces.

---

## PID Process Inspection

The application supports inspection of an individual process.

Example:

```bash
./monitor --pid 298
```

Example output:

```text
==============================
       Process Inspection
==============================
PID:       298
Name:      dockerd
State:     S
Memory:    80.38 MB
Memory %:  1.17%
Threads:   15
==============================
```

Invalid PIDs are handled gracefully:

```bash
./monitor --pid 999999
```

Output:

```text
Process with PID 999999 was not found.
```

---

## Normal Monitoring Mode

Run the application without arguments:

```bash
./monitor
```

Example:

```text
==============================
   Linux System Monitor
==============================

CPU:    [=                   ] 5.2%
Memory: [==                  ] 0.8 GB / 6.7 GB (12.4%)
Swap:   [                    ] 0.0 GB / 2.0 GB (0.0%)

PID     NAME                MEM%    MEM(MB)   THREADS   STATE
------------------------------------------------------------
298     dockerd             1.2     80.4      15        S
223     containerd          0.6     43.9      14        S
225     unattended-upgr     0.5     30.9      2         S
```

Press:

```text
q
```

to stop the monitor.

Press:

```text
e
```

to export the current metrics as JSON.

---

## Project Structure

```text
linux-monitor/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── cpu_monitor.h
│   ├── memory_monitor.h
│   ├── process_monitor.h
│   ├── display.h
│   └── logger.h
│
├── src/
│   ├── main.cpp
│   ├── cpu_monitor.cpp
│   ├── memory_monitor.cpp
│   ├── process_monitor.cpp
│   ├── display.cpp
│   └── logger.cpp
│
├── tests/
│   └── process_monitor_test.cpp
│
└── data/
```

---

## Technology Stack

| Technology | Purpose |
|---|---|
| C++17 | Application development |
| Linux | Runtime environment |
| POSIX APIs | Linux system interaction |
| `/proc` | System/process information |
| CMake | Build system |
| GoogleTest | Unit testing |
| CTest | Test execution |
| GDB | Debugging |
| AddressSanitizer | Memory error detection |
| UBSan | Undefined behavior detection |

---

## Build

### Requirements

- Linux or WSL2
- C++17 compiler
- CMake
- Make
- GoogleTest

### Configure

```bash
mkdir build
cd build
cmake ..
```

### Build

```bash
cmake --build .
```

### Run

```bash
./monitor
```

---

## Testing

The project uses **GoogleTest** for unit testing and **CTest** for automated test execution.

Run the test suite:

```bash
ctest --output-on-failure
```

Current tests cover:

- Current process detection
- Invalid PID handling
- Memory information parsing
- Process state parsing

Example:

```text
Test project /linux-monitor/build

1/4 Test #1: ProcessMonitorTest.FindsCurrentProcess ...... Passed
2/4 Test #2: ProcessMonitorTest.RejectsInvalidPID ........ Passed
3/4 Test #3: ProcessMonitorTest.ReadsMemoryInformation ... Passed
4/4 Test #4: ProcessMonitorTest.ReadsProcessState ........ Passed

100% tests passed, 0 tests failed
```

---

## Debugging

The project can be built with debugging symbols and sanitizers.

Configure a debug build:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-g -fsanitize=address,undefined"
```

Build:

```bash
cmake --build .
```

### GDB

Start GDB:

```bash
gdb ./monitor
```

Example debugging session:

```text
(gdb) break main
(gdb) run
(gdb) bt
(gdb) continue
```

The application can therefore be inspected at runtime using breakpoints and stack traces.

### Sanitizers

The project supports:

```text
AddressSanitizer
UndefinedBehaviorSanitizer
```

These can be used to detect memory-related errors and undefined behavior during development and testing.

---

## Error Handling

The application handles invalid or unavailable process information without terminating the monitoring system.

For example:

```bash
./monitor --pid 999999
```

returns:

```text
Process with PID 999999 was not found.
```

The `/proc` filesystem can change while the application is running, so process files may disappear between discovery and inspection. The implementation checks file availability before parsing process information.

---

## Design Considerations

### Thread Safety

Monitoring threads update shared structures while the display and logging components read them.

Mutexes are used to protect shared state:

```cpp
std::mutex
std::lock_guard
```

Application shutdown is controlled using:

```cpp
std::atomic<bool>
```

### Resource Management

Threads are explicitly joined during shutdown rather than being left detached.

### Linux Process Model

Process information is obtained dynamically from `/proc`, allowing the monitor to work with processes created or terminated while the application is running.

---

## Key Concepts Demonstrated

This project demonstrates practical experience with:

- C++17
- Linux system interfaces
- POSIX APIs
- `/proc` filesystem
- Multithreading
- Mutexes
- Atomics
- Thread lifecycle management
- File parsing
- Process inspection
- Error handling
- CMake
- Unit testing
- Regression testing
- GDB
- AddressSanitizer
- UndefinedBehaviorSanitizer

---

## Future Improvements

Possible extensions include:

- Per-process CPU utilization
- ncurses terminal interface
- Resource threshold alerts
- Historical metric logging
- Additional process statistics
- Configurable monitoring intervals
- Extended automated test coverage

---

## Purpose

This project was developed to strengthen practical understanding of Linux system programming, C++ software development, concurrency, testing, and debugging.
