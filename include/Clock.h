#ifndef INFLUXDATA_INFLUXCLOCK_H
#define INFLUXDATA_INFLUXCLOCK_H

#include <chrono>
#include <ratio>

namespace influxdb {

struct Clock {
    using rep = int64_t;
    using period = std::nano;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<Clock>;
    static constexpr bool is_steady = std::chrono::system_clock::is_steady;
    static time_point now();
};

} // namespace influxdb

#endif
