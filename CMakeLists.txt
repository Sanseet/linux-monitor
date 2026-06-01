cmake_minimum_required(VERSION 3.15)
project(linux_monitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(monitor
    src/main.cpp
    src/cpu_monitor.cpp
    src/memory_monitor.cpp
    src/process_monitor.cpp
    src/display.cpp
    src/logger.cpp
)

target_compile_options(monitor PRIVATE -Wall -Wextra -O2)
target_link_libraries(monitor pthread)
