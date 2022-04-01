#pragma once
#include <chrono>

namespace pgre {
class timer_t
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;

public:
    timer_t();
    void reset();
    std::chrono::seconds seconds();
    std::chrono::milliseconds milliseconds();
};
} // namespace pgre
