#ifndef INFLUXDATA_QUERYRESULT_H
#define INFLUXDATA_QUERYRESULT_H

#include <optional>
#include <map>
#include <sstream>
#include <string>

namespace influxdb {

struct QueryResult {
    const std::string& measurement() const { return _measurement; }

    const std::chrono::time_point<std::chrono::system_clock> time() const { return _timestamp; }

    template <typename T>
    std::optional<T> value(const std::string& key) const {
        if (_values.count(key) == 0) return {};
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
    std::chrono::time_point<std::chrono::system_clock> _timestamp;
};

}

#endif
