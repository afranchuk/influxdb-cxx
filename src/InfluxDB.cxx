///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include "Point.h"
#include "QueryResult.h"

#include <iostream>
#include <memory>
#include <string>

#include "json.hpp"

namespace influxdb
{

InfluxDB::InfluxDB(std::unique_ptr<Transport> transport) :
  mTransport(std::move(transport))
{
  mBuffer = {};
  mBuffering = false;
  mBufferSize = 0;
}

void InfluxDB::batchOf(const std::size_t size)
{
  mBufferSize = size;
  mBuffering = true;
}

void InfluxDB::flushBuffer() {
  if (!mBuffering) {
    return;
  }
  std::string stringBuffer{};
  for (const auto &i : mBuffer) {
    stringBuffer+= i + "\n";
  }
  mBuffer.clear();
  transmit(std::move(stringBuffer));
}

InfluxDB::~InfluxDB()
{
  if (mBuffering) {
    flushBuffer();
  }
}

void InfluxDB::transmit(std::string&& point)
{
  mTransport->send(std::move(point));
}

void InfluxDB::write(Point&& metric)
{
  if (mBuffering) {
    mBuffer.emplace_back(metric.toLineProtocol());
    if (mBuffer.size() >= mBufferSize) {
      flushBuffer();
    }
  } else {
    transmit(metric.toLineProtocol());
  }
}

std::vector<QueryResult> InfluxDB::query(const std::string& query)
{
  auto response = mTransport->query(query);
  std::stringstream ss;
  ss << response;
  std::vector<QueryResult> points;

  auto root = nlohmann::json::parse(response);

  for (auto& result : root["results"]) {
    auto series = result["series"];
    if (series.is_null()) return {};
    for (auto& s : series) {
      auto columns = s["columns"];
      std::string name = s["name"];

      for (auto& values : s["values"]) {
        QueryResult point;
        point._measurement = name;
        auto ic = columns.begin();
        auto iv = values.begin();
        for (; ic != columns.end() && iv != values.end(); ic++, iv++) {
          std::string col = *ic;
          if (iv->is_number_integer() && col == "time") {
              point._timestamp = Clock::time_point(std::chrono::nanoseconds(*iv));
              continue;
          }
          point._values.emplace(col, iv->is_string() ? (std::string)*iv : iv->dump());
        }
        points.push_back(std::move(point));
      }
    }
  }
  return points;
}

} // namespace influxdb
