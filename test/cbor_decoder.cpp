#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace std::string_literals;
using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;

TEST_CASE("cbor throws exception on unsupported tag")
{
  REQUIRE_THROWS_AS(cbor::decode("c0"_hex), cbor::unsupported);
  REQUIRE_THROWS_AS(cbor::decode("e0"_hex), cbor::unsupported);
  SECTION("infinite lenght collections")
  {
    REQUIRE_THROWS_AS(cbor::decode("5f"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("7f"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("9f"_hex), cbor::unsupported);
    REQUIRE_THROWS_AS(cbor::decode("bf"_hex), cbor::unsupported);
  }
}

TEST_CASE("cbor can decode positive integers")
{
  SECTION("throws exception when empty buffer is passed")
  {
    REQUIRE_THROWS_AS(cbor::decode(""_hex), cbor::truncation_error);
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
    REQUIRE_THROWS_AS(cbor::decode("18"_hex), cbor::truncation_error);
    REQUIRE(cbor::decode("1800"_hex) == 0x00);
    REQUIRE(cbor::decode("1818"_hex) == 0x18);
    REQUIRE(cbor::decode("18ff"_hex) == 0xff);
  }
  SECTION("two additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1900"_hex), cbor::truncation_error);
    REQUIRE(cbor::decode("190100"_hex) == 0x0100);
    REQUIRE(cbor::decode("1903e8"_hex) == 0x03e8);
  }
  SECTION("four additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1a000f42"_hex), cbor::truncation_error);
    REQUIRE(cbor::decode("1aeb2f4240"_hex) == 0xeb2f4240);
  }
  SECTION("eight additional bytes integer")
  {
    REQUIRE_THROWS_AS(cbor::decode("1b00000000000000"_hex), cbor::truncation_error);
    REQUIRE(cbor::decode("1b1122334455667788"_hex) == 0x1122334455667788);
  }
  SECTION("decoding with leftovers")
  {
    auto const bytes = "171818191919"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
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
    REQUIRE_THROWS_AS(cbor::decode("38"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("3900"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("3a000000"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("3b00000000000000"_hex), cbor::truncation_error);
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
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    REQUIRE(json == -0x18);
    json = cbor::decode(leftovers);
    REQUIRE(json == -0x19);
    json = cbor::decode(leftovers);
    REQUIRE(json == -0x0100);
  }
}

TEST_CASE("cbor can decode byte_stream")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("41"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("44000000"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("59000200"_hex), cbor::truncation_error);
  }
  REQUIRE(cbor::decode("40"_hex) == ""_hex);
  REQUIRE(cbor::decode("4100"_hex) == "00"_hex);
  REQUIRE(cbor::decode("41ff"_hex) == "ff"_hex);
  REQUIRE(cbor::decode("5803112233"_hex) == "112233"_hex);
  SECTION("decoding with leftovers")
  {
    auto const bytes = "411158022233590003445566"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    REQUIRE(json == "11"_hex);
    json = cbor::decode(leftovers);
    REQUIRE(json == "2233"_hex);
    json = cbor::decode(leftovers);
    REQUIRE(json == "445566"_hex);
  }
}

TEST_CASE("cbor can decode unicode")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("61"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("64000000"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("79000200"_hex), cbor::truncation_error);
  }
  REQUIRE(cbor::decode("60"_hex) == ""s);
  REQUIRE(cbor::decode("6161"_hex) == "a");
  REQUIRE(cbor::decode("6449455446"_hex) == "IETF");
  REQUIRE(cbor::decode("62225c"_hex) == "\"\\");
  REQUIRE(cbor::decode("62c3bc"_hex) == "ü");
  REQUIRE(cbor::decode("63e6b0b4"_hex) == "水");
  REQUIRE(cbor::decode("64f0908591"_hex) == "𐅑");
  REQUIRE(cbor::decode("656c6f72656d"_hex) == "lorem");
  REQUIRE(
      cbor::decode("7820697073756d20646f6c6f722073697420616d657420636f6e7365637465747572"_hex) ==
      "ipsum dolor sit amet consectetur");
  SECTION("decoding with leftovers")
  {
    auto const bytes = "656c6f72656d644945544662c3bc"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    REQUIRE(json == "lorem");
    json = cbor::decode(leftovers);
    REQUIRE(json == "IETF");
    json = cbor::decode(leftovers);
    REQUIRE(json == "ü");
  }
}

TEST_CASE("cbor can decode arrays")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("81"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("8417181819ffff"_hex), cbor::truncation_error);
  }
  SECTION("size exceeds limit")
  {
    REQUIRE_THROWS_AS(cbor::decode("9a00010000"_hex), cbor::unsupported);
  }
  SECTION("nesting exceeds limit")
  {
    cxx::json::byte_stream data(cxx::cbor::max_nesting + 2, cxx::byte(0x81));
    REQUIRE_THROWS_AS(cbor::decode(data), cbor::unsupported);
  }

  REQUIRE(cbor::decode("80"_hex) == cxx::json::array());
  SECTION("one item")
  {
    cxx::json::array const array = {0x17};
    REQUIRE(cbor::decode("8117"_hex) == array);
  }
  REQUIRE(cbor::decode("82171818"_hex) == cxx::json::array({0x17, 0x18}));
  REQUIRE(cbor::decode("9803171818656c6f72656d"_hex) == cxx::json::array({0x17, 0x18, "lorem"}));
  REQUIRE(cbor::decode("9803008218ff656c6f72656d6449455446"_hex) ==
          cxx::json::array({0x00, {0xff, "lorem"}, "IETF"}));
  SECTION("decoding with leftovers")
  {
    auto const bytes = "811782181862c3bc9803656c6f72656d2020"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    cxx::json json = cbor::decode(leftovers);
    {
      cxx::json::array const array = {0x17};
      REQUIRE(json == array);
    }
    json = cbor::decode(leftovers);
    REQUIRE(json == cxx::json::array({0x18, "ü"}));
    json = cbor::decode(leftovers);
    REQUIRE(json == cxx::json::array({"lorem", -0x01, -0x01}));
  }
}

TEST_CASE("cbor can decode dictionaties")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("a1"_hex), cbor::truncation_error);
  }
  SECTION("size exceeds limit")
  {
    REQUIRE_THROWS_AS(cbor::decode("ba00010000"_hex), cbor::unsupported);
  }
  SECTION("key type is not unicode")
  {
    REQUIRE_THROWS_AS(cbor::decode("a118181818"_hex), cbor::unsupported);
  }
  SECTION("nesting exceeds limit")
  {
    cxx::json::byte_stream data;
    data.reserve(3 * (cxx::cbor::max_nesting + 3));
    data.push_back(cxx::byte(0xa1));
    while (std::size(data) < 3 * (cxx::cbor::max_nesting + 1))
    {
      data.push_back(cxx::byte(0x61));
      data.push_back(cxx::byte(0x61));
      data.push_back(cxx::byte(0xa1));
    }
    REQUIRE_THROWS_AS(cbor::decode(data), cbor::unsupported);
  }
  SECTION("mix objects nesting exceeds limit")
  {
    cxx::json::byte_stream data;
    data.reserve(4 * (cxx::cbor::max_nesting + 3));
    while (std::size(data) < 4 * (cxx::cbor::max_nesting + 1))
    {
      data.push_back(cxx::byte(0xa1));
      data.push_back(cxx::byte(0x61));
      data.push_back(cxx::byte(0x61));
      data.push_back(cxx::byte(0x81));
    }
    REQUIRE_THROWS_AS(cbor::decode(data), cbor::unsupported);
  }
  REQUIRE(cbor::decode("a0"_hex) == cxx::json::dictionary());
  SECTION("one item")
  {
    REQUIRE(cbor::decode("a161611818"_hex) == cxx::json::dictionary({{"a", 0x18}}));
    REQUIRE(cbor::decode("a1656c6f72656d6449455446"_hex) ==
            cxx::json::dictionary({{"lorem", "IETF"}}));
  }
  REQUIRE(cbor::decode("a261611818656c6f72656d6449455446"_hex) ==
          cxx::json::dictionary({{"a", 0x18}, {"lorem", "IETF"}}));
  REQUIRE(cbor::decode("a2656c6f72656d644945544661611818"_hex) ==
          cxx::json::dictionary({{"a", 0x18}, {"lorem", "IETF"}}));
  SECTION("decoding with leftovers")
  {
    auto const bytes = "a161611818a2616218196163181aa36164181b6165181c6166181d"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(cbor::decode(leftovers) == cxx::json::dictionary({{"a", 0x18}}));
    REQUIRE(cbor::decode(leftovers) == cxx::json::dictionary({{"b", 0x19}, {"c", 0x1a}}));
    REQUIRE(cbor::decode(leftovers) ==
            cxx::json::dictionary({{"d", 0x1b}, {"e", 0x1c}, {"f", 0x1d}}));
  }
}

TEST_CASE("cbor can decode simple special values")
{
  REQUIRE_THROWS_AS(cbor::decode("f8"_hex), cbor::truncation_error);
  REQUIRE(cbor::decode("f4"_hex) == false);
  REQUIRE(cbor::decode("f8f4"_hex) == false);
  REQUIRE(cbor::decode("f5"_hex) == true);
  REQUIRE(cbor::decode("f8f5"_hex) == true);
  REQUIRE(cbor::decode("f6"_hex) == cxx::json::null);
  REQUIRE(cbor::decode("f8f6"_hex) == cxx::json::null);
  SECTION("decoding with leftovers")
  {
    auto const bytes = "f4f8f5f6"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(cbor::decode(leftovers) == false);
    REQUIRE(cbor::decode(leftovers) == true);
    REQUIRE(cbor::decode(leftovers) == cxx::json::null);
  }
}

TEST_CASE("cbor can decode floating point numbers")
{
  SECTION("can identify truncation erros")
  {
    REQUIRE_THROWS_AS(cbor::decode("f900"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("fa000000"_hex), cbor::truncation_error);
    REQUIRE_THROWS_AS(cbor::decode("fb00000000000000"_hex), cbor::truncation_error);
  }
  REQUIRE(cbor::decode("f97bff"_hex) == 65504.0);
  REQUIRE(cbor::decode("fa47c35000"_hex) == 100000.0);
  REQUIRE(cbor::decode("fb0000000000000000"_hex) == 0.0);
  REQUIRE(cbor::decode("fb7e37e43c8800759c"_hex) == 1.0e+300);
  REQUIRE(cbor::decode("fb3ff199999999999a"_hex) == 1.1);
}
