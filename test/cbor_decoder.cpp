#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

TEST_CASE("cbor can decodec integers")
{
  using item = std::pair<cxx::json, cbor::byte_view>;
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
    REQUIRE(cbor::decode("00"_hex) == item{0, cbor::byte_view{}});
    REQUIRE(cbor::decode("01"_hex) == item{1, cbor::byte_view{}});
    REQUIRE(cbor::decode("17"_hex) == item{23, cbor::byte_view{}});
  }
  SECTION("single additional byte integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1c"_hex), cbor::data_error);
    REQUIRE_THROWS_AS(cbor::decode("18"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1800"_hex) == item{0x00, cbor::byte_view{}});
    REQUIRE(cbor::decode("1818"_hex) == item{0x18, cbor::byte_view{}});
    REQUIRE(cbor::decode("18ff"_hex) == item{0xff, cbor::byte_view{}});
  }
  SECTION("two additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1900"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("190100"_hex) == item{0x0100, cbor::byte_view{}});
    REQUIRE(cbor::decode("1903e8"_hex) == item{0x03e8, cbor::byte_view{}});
  }
  SECTION("two additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1a000f42"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1aeb2f4240"_hex) == item{0xeb2f4240, cbor::byte_view{}});
  }
}
