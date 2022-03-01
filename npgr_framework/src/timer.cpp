#include "timer.h"

namespace npgr {
timer_t::timer_t() { reset(); }

void timer_t::reset() {
    _start_time = std::chrono::high_resolution_clock::now();
}

std::chrono::seconds timer_t::seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()
                                                                 - _start_time);
}

std::chrono::milliseconds timer_t::milliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                                                                 - _start_time);
}

} // namespace npgr