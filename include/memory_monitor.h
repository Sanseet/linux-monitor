#pragma once

struct MemoryStats {
    long long total_kb;
    long long available_kb;
    long long used_kb;
    long long swap_total_kb;
    long long swap_free_kb;
    long long swap_used_kb;
    double usage_percent;
    double swap_percent;
};

class MemoryMonitor {
public:
    MemoryStats getStats();
};
