#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

TEST_CASE("cbor throws exception on unsupported tag")
{
  REQUIRE_THROWS_AS(cbor::decode("40"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("60"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("80"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("a0"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("c0"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("e0"_hex), cbor::unsupported);
}

TEST_CASE("cbor can decode positive integers")
{
  SECTION("throws exception when empty buffer is passed")
  {
    REQUIRE_THROWS_AS(cbor::decode(""_hex), cbor::buffer_error);
  }
  SECTION("initial byte integer")
  {
    REQUIRE(cbor::decode("00"_hex) == 0x00);
    REQUIRE(cbor::decode("01"_hex) == 0x01);
    REQUIRE(cbor::decode("17"_hex) == 0x17);
  }
  SECTION("single additional byte integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1c"_hex), cbor::data_error);
    REQUIRE_THROWS_AS(cbor::decode("18"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1800"_hex) == 0x00);
    REQUIRE(cbor::decode("1818"_hex) == 0x18);
    REQUIRE(cbor::decode("18ff"_hex) == 0xff);
  }
  SECTION("two additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1900"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("190100"_hex) == 0x0100);
    REQUIRE(cbor::decode("1903e8"_hex) == 0x03e8);
  }
  SECTION("four additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1a000f42"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1aeb2f4240"_hex) == 0xeb2f4240);
  }
  SECTION("eight additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1b00000000000000"_hex), cbor::buffer_error);
    REQUIRE(cbor::decode("1b1122334455667788"_hex) == 0x1122334455667788);
  }
  SECTION("decoding with leftovers")
  {
    auto const bytes = "171818191919"_hex;
    cbor::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    REQUIRE(json == 0x17);
    REQUIRE(std::size(leftovers) == 5);
    json = cbor::decode(leftovers);
    REQUIRE(json == 0x18);
    REQUIRE(std::size(leftovers) == 3);
    json = cbor::decode(leftovers);
    REQUIRE(json == 0x1919);
    REQUIRE(std::empty(leftovers));
  }
}

TEST_CASE("cbor can decode negative integers")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("38"_hex), cbor::buffer_error);
    REQUIRE_THROWS_AS(cbor::decode("3900"_hex), cbor::buffer_error);
    REQUIRE_THROWS_AS(cbor::decode("3a000000"_hex), cbor::buffer_error);
    REQUIRE_THROWS_AS(cbor::decode("3b00000000000000"_hex), cbor::buffer_error);
  }
  REQUIRE(cbor::decode("20"_hex) == -0x01);
  REQUIRE(cbor::decode("37"_hex) == -0x18);
  REQUIRE(cbor::decode("3818"_hex) == -0x19);
  REQUIRE(cbor::decode("38ff"_hex) == -0x0100);
  REQUIRE(cbor::decode("390100"_hex) == -0x101);
  REQUIRE(cbor::decode("39ffff"_hex) == -0x10000);
  REQUIRE(cbor::decode("3a00010000"_hex) == -0x10001);
  REQUIRE(cbor::decode("3affffffff"_hex) == -0x100000000);
  REQUIRE(cbor::decode("3b7fffffffffffffff"_hex) == std::numeric_limits<std::int64_t>::min());
  SECTION("decoding with leftovers")
  {
    auto const bytes = "37381838ff"_hex;
    cbor::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    REQUIRE(json == -0x18);
    json = cbor::decode(leftovers);
    REQUIRE(json == -0x19);
    json = cbor::decode(leftovers);
    REQUIRE(json == -0x0100);
  }
}
