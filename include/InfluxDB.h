///
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_INFLUXDB_H
#define INFLUXDATA_INFLUXDB_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <deque>

#include "Transport.h"

namespace influxdb
{

class Point;
struct QueryResult;

class InfluxDB
{
  public:
    /// Disable copy constructor
    InfluxDB & operator=(const InfluxDB&) = delete;

    /// Disable copy constructor
    InfluxDB(const InfluxDB&) = delete;

    /// Constructor required valid transport
    InfluxDB(std::unique_ptr<Transport> transport);

    /// Flushes buffer
    ~InfluxDB();

    /// Writes a metric
    /// \param metric
    void write(Point&& metric);

    /// Queries InfluxDB database
    std::vector<QueryResult> query(const std::string& query);

    /// Flushes metric buffer (this can also happens when buffer is full)
    void flushBuffer();

    /// Enables metric buffering
    /// \param size
    void batchOf(const std::size_t size = 32);

  private:
    /// Buffer for points
    std::deque<std::string> mBuffer;

    /// Flag stating whether point buffering is enabled
    bool mBuffering;

    /// Buffer size
    std::size_t mBufferSize;

    /// Underlying transport UDP/HTTP/Unix socket
    std::unique_ptr<Transport> mTransport;

    /// Transmits string over transport
    void transmit(std::string&& point);
};

} // namespace influxdb

#endif // INFLUXDATA_INFLUXDB_H
