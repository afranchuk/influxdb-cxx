#define BOOST_TEST_MODULE Test InfluxDB Query
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../include/InfluxDBFactory.h"
#include "../include/QueryResult.h"

namespace influxdb {
namespace test {



BOOST_AUTO_TEST_CASE(query1)
{
  auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086?db=test");
  auto points = influxdb->query("SELECT * from test LIMIT 2");
  BOOST_CHECK_EQUAL(points.front().measurement(), "test");
  BOOST_CHECK_EQUAL(points.back().measurement(), "test");
  auto first = points.front().value<int>("value");
  BOOST_REQUIRE(first);
  BOOST_CHECK_EQUAL(first.value(), 10);
  auto last = points.back().value<int>("value");
  BOOST_REQUIRE(last);
  BOOST_CHECK_EQUAL(last.value(), 20);
}

} // namespace test
} // namespace influxdb
