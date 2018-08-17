#include "inc/cxx/msgpack.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace std::string_literals;
using namespace cxx::literals;
using namespace test::literals;

using msgpack = cxx::msgpack;

TEST_CASE("msgpack throws exception on unsupported tag")
{
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

TEST_CASE("msgpack can decode integers")
{
  SECTION("truncation_error")
  {
    REQUIRE_THROWS_AS(msgpack::decode("cc"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("ce00"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("ce000000"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("cf00000000000000"_hex), msgpack::truncation_error);

    REQUIRE_THROWS_AS(msgpack::decode("d0"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("d1ff"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("d2ffffff"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("d3ffffffffffffff"_hex), msgpack::truncation_error);
  }

  SECTION("limits")
  {
    REQUIRE_THROWS_AS(msgpack::decode("cfffffffffffffffff"_hex), msgpack::unsupported);
  }

  REQUIRE(msgpack::decode("00"_hex) == 0x00);
  REQUIRE(msgpack::decode("7f"_hex) == 0x7f);
  REQUIRE(msgpack::decode("cc00"_hex) == 0x00);
  REQUIRE(msgpack::decode("ccff"_hex) == 0xff);
  REQUIRE(msgpack::decode("cd0000"_hex) == 0x00);
  REQUIRE(msgpack::decode("cdffff"_hex) == 0xffff);
  REQUIRE(msgpack::decode("ce00000000"_hex) == 0x00);
  REQUIRE(msgpack::decode("ceffffffff"_hex) == 0xffffffff);
  REQUIRE(msgpack::decode("cf0000000000000000"_hex) == 0x00);
  REQUIRE(msgpack::decode("cf1fffffffffffffff"_hex) == 0x1fffffffffffffff);
  REQUIRE(msgpack::decode("cf7fffffffffffffff"_hex) == std::numeric_limits<std::int64_t>::max());

  REQUIRE(msgpack::decode("d000"_hex) == 0x00);
  REQUIRE(msgpack::decode("d001"_hex) == 0x01);
  REQUIRE(msgpack::decode("d080"_hex) == -0x80);
  REQUIRE(msgpack::decode("d0ff"_hex) == -0x01);

  REQUIRE(msgpack::decode("d10000"_hex) == 0x00);
  REQUIRE(msgpack::decode("d10001"_hex) == 0x01);
  REQUIRE(msgpack::decode("d18000"_hex) == -0x8000);
  REQUIRE(msgpack::decode("d18fff"_hex) == -0x7001);
  REQUIRE(msgpack::decode("d1ffff"_hex) == -0x01);

  REQUIRE(msgpack::decode("d200000000"_hex) == 0x00);
  REQUIRE(msgpack::decode("d200000001"_hex) == 0x01);
  REQUIRE(msgpack::decode("d280000000"_hex) == -0x80000000l);
  REQUIRE(msgpack::decode("d28fffffff"_hex) == -0x70000001l);
  REQUIRE(msgpack::decode("d2ffffffff"_hex) == -0x01);

  REQUIRE(msgpack::decode("d30000000000000000"_hex) == 0x00);
  REQUIRE(msgpack::decode("d30000000000000001"_hex) == 0x01);
  REQUIRE(msgpack::decode("d37fffffffffffffff"_hex) == std::numeric_limits<std::int64_t>::max());
  REQUIRE(msgpack::decode("d38000000000000000"_hex) == std::numeric_limits<std::int64_t>::min());
  REQUIRE(msgpack::decode("d38fffffffffffffff"_hex) == -0x7000000000000001l);
  REQUIRE(msgpack::decode("d3ffffffffffffffff"_hex) == -0x01);
}
