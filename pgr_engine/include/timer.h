#pragma once
#include <chrono>

namespace pgre {

struct interval_t {
    int64_t milliseconds;
    float seconds;
} __attribute__((aligned(16)));

class timer_t
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;

public:
    timer_t();
    void reset();
    interval_t get_interval();
    float get_seconds();
    int64_t get_milliseconds();
};


} // namespace pgre
