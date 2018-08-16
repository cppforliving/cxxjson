#include "inc/cxx/msgpack.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace std::string_literals;
using namespace cxx::literals;
using namespace test::literals;

using msgpack = cxx::msgpack;

TEST_CASE("msgpack throws exception on unsupported tag")
{
  REQUIRE_THROWS_AS(msgpack::decode("00"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c0"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c2"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c3"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c4"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c5"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c6"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c7"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c8"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("c9"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("ca"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("cb"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("cc"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("cd"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("ce"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("cf"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d0"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d1"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d2"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d3"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d4"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d5"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d6"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d7"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d8"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("d9"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("da"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("db"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("dc"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("dd"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("de"_hex), msgpack::unsupported);
  REQUIRE_THROWS_AS(msgpack::decode("df"_hex), msgpack::unsupported);
}
