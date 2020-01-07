///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Point.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

namespace influxdb {

static std::string escape(std::string_view w, const char *chars)
{
    std::string ret;
    ret.reserve(w.size());
    size_t pos = 0;
    while (true) {
        auto end = w.find_first_of(chars, pos);
        if (end == std::string_view::npos)
            break;
        ret += w.substr(pos, end - pos);
        ret += '\\';
        pos = end;
    }
    ret += w.substr(pos);
    return ret;
}

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Point::Point(std::string measurement) : _measurement(std::move(measurement)), mTimestamp(std::chrono::system_clock::now()) {}

Point &&Point::addField(std::string name, Point::field_value_type value)
{
    _fields.emplace(std::move(name), std::move(value));
    return std::move(*this);
}

Point &&Point::addTag(std::string key, std::string value)
{
    _tags.emplace(std::move(key), std::move(value));
    return std::move(*this);
}

Point &&Point::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
    mTimestamp = std::move(timestamp);
    return std::move(*this);
}

std::string Point::toLineProtocol() const
{
    std::stringstream ss;
    ss << escape(_measurement, ", ");
    for (const auto &[k, v] : _tags) {
        ss << "," << escape(k, "=, ") << "=" << escape(v, "=, ");
    }
    bool has_fields = false;
    for (const auto &[k, v] : _fields) {
        ss << (has_fields ? "," : " ") << escape(k, "=, ") << "=";
        std::visit(overloaded{
                       [&ss](bool value) { ss << (value ? "true" : "false"); },
                       [&ss](int value) { ss << value << 'i'; },
                       [&ss](double value) { ss << value; },
                       [&ss](const std::string &value) { ss << '"' << escape(value, "\"\\") << '"'; },
                   },
                   v);
        has_fields = true;
    }
    if (!has_fields)
        throw std::runtime_error("cannot convert to line protocol; no fields specified");
    ss << " "
       << std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count());

    return ss.str();
}

const std::string *Point::tag(const std::string& key) const
{
    if (_tags.count(key) == 0)
        return nullptr;

    return &_tags.at(key);
}

const Point::field_value_type *Point::field(const std::string& key) const
{
    if (_fields.count(key) == 0)
        return nullptr;

    return &_fields.at(key);
}

} // namespace influxdb
