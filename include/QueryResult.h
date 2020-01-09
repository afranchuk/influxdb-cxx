#ifndef INFLUXDATA_QUERYRESULT_H
#define INFLUXDATA_QUERYRESULT_H

#include <map>
#include <optional>
#include <sstream>
#include <string>

#include "Clock.h"

namespace influxdb {

/// Represents a single query result.
struct QueryResult {
    /// Get the measurement name for this result.
    const std::string &measurement() const { return _measurement; }

    /// Get the time for this result.
    const Clock::time_point timestamp() const { return _timestamp; }

    /// Get the value related to the given key for this result.
    ///
    /// This accesses both tags and fields in a result.
    /// `operator>>(istream&,T&)` must be implemented.
    template <typename T> std::optional<T> value(const std::string &key) const
    {
        if (_values.count(key) == 0)
            return {};
        T v;
        std::stringstream ss(_values.at(key));
        ss >> v;
        return v;
    }

  private:
    friend class InfluxDB;

    QueryResult() = default;

    std::string _measurement;
    std::map<std::string, std::string> _values;
    Clock::time_point _timestamp;
};

} // namespace influxdb

#endif
