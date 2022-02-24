#pragma once
#include <chrono>

namespace npgr {
class timer_t
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;

public:
    timer_t();
    void reset();
    int64_t seconds();
    int64_t milliseconds();
};
} // namespace npgr
