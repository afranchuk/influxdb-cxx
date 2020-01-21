///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "HTTP.h"
#include <iostream>

namespace influxdb {
namespace transports {

HTTP::HTTP(const std::string &url, int port, const std::string &db) : client(url, port), db(db) {}

std::string HTTP::query(const std::string &query)
{
    httplib::Params params{{"db", db}, {"epoch", "ns"}, {"q", query}};
    auto res = client.Post("/query", params);
    if (res && res->status == 200) {
        return res->body;
    }
    else if (res) {
        throw std::runtime_error("Got status " + std::to_string(res->status) + ": " + res->body);
    }
    else {
        throw std::runtime_error("failed to get response");
    }
}

void HTTP::enableBasicAuth(const std::string &auth)
{
    auto pos = auth.find(':');
    if (pos != std::string::npos) {
        auto username = auth.substr(0, pos);
        auto password = auth.substr(pos + 1);
        client.set_basic_auth(username.c_str(), password.c_str());
    }
    else {
        throw std::runtime_error("invalid auth string");
    }
}

void HTTP::enableSsl() { throw std::logic_error("unimplemented"); }

HTTP::~HTTP() {}

void HTTP::send(std::string &&post)
{
    auto res = client.Post(("/write?db=" + db).c_str(), post, "multipart/form-data");
    if (!res || res->status != 204) {
        throw std::runtime_error("failed to write post data" + (res ? ": " + res->body : ""));
    }
}

} // namespace transports
} // namespace influxdb
