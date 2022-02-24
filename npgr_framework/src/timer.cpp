#include "timer.h"

namespace npgr {
timer_t::timer_t() { reset(); }

void timer_t::reset() {}

int64_t timer_t::seconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _start_time)
             .count()
           * 0.001F * 0.001F * 0.001F;
}

int64_t timer_t::milliseconds() { return seconds() * 1000.0F; }

} // namespace npgr