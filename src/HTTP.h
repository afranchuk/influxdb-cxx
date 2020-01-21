///
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_TRANSPORTS_HTTP_H
#define INFLUXDATA_TRANSPORTS_HTTP_H

#include "Transport.h"
#include "httplib.h"
#include <memory>
#include <string>

namespace influxdb
{
namespace transports
{

/// \brief HTTP transport
class HTTP : public Transport
{
  public:
    /// Constructor
    HTTP(const std::string& url, int port, const std::string& db);

    /// Default destructor
    ~HTTP();

    /// Sends point via HTTP POST
    void send(std::string&& post) override;

    /// Queries database
    std::string query(const std::string& query) override;

    /// Enable Basic Auth
    /// \param auth <username>:<password>
    void enableBasicAuth(const std::string& auth);

    /// Enable SSL
    void enableSsl();
  private:
    httplib::Client client;

    /// InfluxDB read URL
    std::string db;
};

} // namespace transports
} // namespace influxdb

#endif // INFLUXDATA_TRANSPORTS_HTTP_H
