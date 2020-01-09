#include "Clock.h"

namespace influxdb {

Clock::time_point Clock::now() {
    auto now = std::chrono::system_clock::now();
    // Assume system_clock has an epoch of 1/1/1970. Guaranteed in C++20.
    return Clock::time_point(std::chrono::duration_cast<Clock::duration>(now.time_since_epoch()));
}

} // namespace influxdb
