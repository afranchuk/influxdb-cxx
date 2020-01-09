///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB.h"
#include "Point.h"
#include "QueryResult.h"

#include <iostream>
#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  for (auto& result : pt.get_child("results")) {
    auto isResultEmpty = result.second.find("series");
    if (isResultEmpty == result.second.not_found()) return {};
    for (auto& series : result.second.get_child("series")) {
      auto columns = series.second.get_child("columns");

      for (auto& values : series.second.get_child("values")) {
        QueryResult point;
        point._measurement = series.second.get<std::string>("name");
        auto iColumns = columns.begin();
        auto iValues = values.second.begin();
        for (; iColumns != columns.end() && iValues != values.second.end(); iColumns++, iValues++) {
          auto value = iValues->second.get_value<std::string>();
          auto column = iColumns->second.get_value<std::string>();
          if (column == "time") {
            std::stringstream ss;
            ss << value;
            size_t ns = 0;
            ss >> ns;
            point._timestamp = Clock::time_point(std::chrono::nanoseconds(ns));
            continue;
          }
          point._values.emplace(column, value);
        }
        points.push_back(std::move(point));
      }
    }
  }
  return points;
}

} // namespace influxdb
