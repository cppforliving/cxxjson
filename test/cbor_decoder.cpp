#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

TEST_CASE("cbor can decodec integers")
{
  SECTION("throws exception when empty buffer is passed")
  {
    REQUIRE_THROWS_AS(cbor::decode(""_hex), cbor::buffer_error);
  }
  SECTION("throws exception when decoding unsupported type")
  {
    REQUIRE_THROWS_AS(cbor::decode("20"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("40"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("60"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("80"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("a0"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("c0"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("e0"_hex), cbor::unsupported);
  }
  SECTION("initial byte integer")
  {
    REQUIRE(cbor::decode("00"_hex).first == 0);
    REQUIRE(cbor::decode("00"_hex).second.empty());
    REQUIRE(cbor::decode("01"_hex).first == 1);
    REQUIRE(cbor::decode("01"_hex).second.empty());
    REQUIRE(cbor::decode("17"_hex).first == 23);
    REQUIRE(cbor::decode("17"_hex).second.empty());
  }
  SECTION("single additional byte integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1c"_hex), cbor::data_error);
    REQUIRE_THROWS_AS(cbor::decode("18"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1800"_hex).first == 0x00);
    REQUIRE(cbor::decode("1800"_hex).second.empty());
    REQUIRE(cbor::decode("1818"_hex).first == 0x18);
    REQUIRE(cbor::decode("1818"_hex).second.empty());
    REQUIRE(cbor::decode("18ff"_hex).first == 0xff);
    REQUIRE(cbor::decode("18ff"_hex).second.empty());
  }
}
