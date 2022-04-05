#include <timer.h>

namespace pgre {
timer_t::timer_t() { reset(); }

void timer_t::reset() {
    _start_time = std::chrono::high_resolution_clock::now();
}

float timer_t::get_seconds() {
    return static_cast<float>(get_milliseconds())/1000;
}

int64_t timer_t::get_milliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                                                                 - _start_time).count();
}

interval_t timer_t::get_interval() {
    return {get_milliseconds(), get_seconds()};
}

} // namespace pgre