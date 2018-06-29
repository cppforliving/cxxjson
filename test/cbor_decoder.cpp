#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

namespace
{
  auto item(cxx::json j)
  {
    return std::pair<cxx::json, cbor::byte_view>(std::move(j), cbor::byte_view{});
  }
}

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
    REQUIRE(cbor::decode("00"_hex) == item(0x00));
    REQUIRE(cbor::decode("01"_hex) == item(0x01));
    REQUIRE(cbor::decode("17"_hex) == item(0x17));
  }
  SECTION("single additional byte integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1c"_hex), cbor::data_error);
    REQUIRE_THROWS_AS(cbor::decode("18"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1800"_hex) == item(0x00));
    REQUIRE(cbor::decode("1818"_hex) == item(0x18));
    REQUIRE(cbor::decode("18ff"_hex) == item(0xff));
  }
  SECTION("two additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1900"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("190100"_hex) == item(0x0100));
    REQUIRE(cbor::decode("1903e8"_hex) == item(0x03e8));
  }
  SECTION("four additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1a000f42"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1aeb2f4240"_hex) == item(0xeb2f4240));
  }
  SECTION("eight additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1b00000000000000"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1b1122334455667788"_hex) == item(0x1122334455667788));
  }
  SECTION("decoding with leftovers")
  {
    auto const data = "171818191919"_hex;
    auto[json, leftovers] = cbor::decode(data);
    REQUIRE(json == 0x17);
    REQUIRE(std::size(leftovers) == 5);
    std::tie(json, leftovers) = cbor::decode(leftovers);
    REQUIRE(json == 0x18);
    REQUIRE(std::size(leftovers) == 3);
    std::tie(json, leftovers) = cbor::decode(leftovers);
    REQUIRE(json == 0x1919);
    REQUIRE(std::empty(leftovers));
  }
}
