///
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_POINT_H
#define INFLUXDATA_POINT_H

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <variant>

#include "Clock.h"

namespace influxdb
{

/// \brief Represents a point
class Point
{
  public:
    using field_value_type = std::variant<int, double, std::string, bool>;

    /// Constructs point based on measurement name
    Point(std::string measurement);

    /// Default destructor
    ~Point() = default;

    /// Adds a tags
    Point&& addTag(std::string key, std::string value);

    /// Adds filed
    Point&& addField(std::string name, field_value_type value);

    /// Sets custom timestamp
    Point&& setTimestamp(Clock::time_point timestamp);

    /// Converts point to Influx Line Protocol
    std::string toLineProtocol() const;

    /// Get the measurement.
    const std::string& measurement() const { return _measurement; }

    /// Get the point tags.
    const std::map<std::string, std::string>& tags() const { return _tags; }

    /// Get the point fields.
    const std::map<std::string, field_value_type>& fields() const { return _fields; }

    /// Get a specific tag.
    ///
    /// Returns nullptr if no tag exists.
    const std::string* tag(const std::string& key) const;

    /// Get a specific field.
    ///
    /// Returns nullptr if no field exists.
    const field_value_type* field(const std::string& key) const;

    /// Get the timestamp.
    const Clock::time_point& timestamp() const { return _timestamp; }

  protected:
    /// The point measurement name.
    const std::string _measurement;

    /// Point tags.
    std::map<std::string, std::string> _tags;

    /// Point fields.
    std::map<std::string, field_value_type> _fields;

    /// A timestamp
    Clock::time_point _timestamp;
};

} // namespace influxdb

#endif // INFLUXDATA_POINT_H
