# Linux System Monitor

A lightweight Linux system monitoring tool built in C++ using POSIX APIs and the Linux `/proc` filesystem.

## Features

- Real-time CPU monitoring
- Memory monitoring
- Swap monitoring
- Process inspection
- Thread tracking
- Process state analysis
- Live terminal dashboard
- JSON export support

## Architecture

```
/proc/stat      -> CPU Monitor
/proc/meminfo   -> Memory Monitor
/proc/[pid]     -> Process Monitor
                    ↓
             Metrics Engine
                    ↓
             Terminal Dashboard
                    ↓
               JSON Export
```

## Tech Stack

- C++17
- Linux
- POSIX APIs
- CMake

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
./linux-monitor
```

## Sample Output

```
CPU:    12.4%
Memory: 0.6 GB / 6.7 GB
Swap:   0.0 GB / 2.0 GB

PID     NAME       MEM%   THREADS
2477    dockerd    1.2    16
2282    containerd 0.7    14
```

## Future Enhancements

- Multithreaded metric collection
- Historical metric logging
- Resource usage alerts
- ncurses dashboard
- REST API integration
