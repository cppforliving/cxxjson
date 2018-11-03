#include "inc/cxx/msgpack.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace std::string_literals;
using namespace cxx::literals;
using namespace test::literals;

using msgpack = cxx::msgpack;

TEST_CASE("msgpack throws exception on unsupported tag")
{
  REQUIRE_THROWS_AS(msgpack::decode("c7"_hex), msgpack::unsupported); // ext8
  REQUIRE_THROWS_AS(msgpack::decode("c8"_hex), msgpack::unsupported); // ext16
  REQUIRE_THROWS_AS(msgpack::decode("c9"_hex), msgpack::unsupported); // ext32
  REQUIRE_THROWS_AS(msgpack::decode("ca"_hex), msgpack::unsupported); // float32
  REQUIRE_THROWS_AS(msgpack::decode("cb"_hex), msgpack::unsupported); // float64
  REQUIRE_THROWS_AS(msgpack::decode("d4"_hex), msgpack::unsupported); // fixext1
  REQUIRE_THROWS_AS(msgpack::decode("d5"_hex), msgpack::unsupported); // fixext2
  REQUIRE_THROWS_AS(msgpack::decode("d6"_hex), msgpack::unsupported); // fixext4
  REQUIRE_THROWS_AS(msgpack::decode("d7"_hex), msgpack::unsupported); // fixext8
  REQUIRE_THROWS_AS(msgpack::decode("d8"_hex), msgpack::unsupported); // fixext16
  REQUIRE_THROWS_AS(msgpack::decode("de"_hex), msgpack::unsupported); // map16
  REQUIRE_THROWS_AS(msgpack::decode("df"_hex), msgpack::unsupported); // map32
}

TEST_CASE("msgpack throws truncation_error on empty buffer")
{
  REQUIRE_THROWS_AS(msgpack::decode(""_hex), msgpack::truncation_error);
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

  REQUIRE(msgpack::decode("e0"_hex) == -0x20);
  REQUIRE(msgpack::decode("ef"_hex) == -0x11);
  REQUIRE(msgpack::decode("ff"_hex) == -0x01);
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

  SECTION("leftovers")
  {
    auto const bytes = "ef7fcdff77d18fff00"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(msgpack::decode(leftovers) == -0x11);
    REQUIRE(msgpack::decode(leftovers) == 0x7f);
    REQUIRE(msgpack::decode(leftovers) == 0xff77);
    REQUIRE(msgpack::decode(leftovers) == -0x7001);
    REQUIRE(std::size(leftovers) == 1);
  }
}

TEST_CASE("msgpack can decode simple values")
{
  REQUIRE(msgpack::decode("c0"_hex) == cxx::json::null);
  REQUIRE(msgpack::decode("c2"_hex) == false);
  REQUIRE(msgpack::decode("c3"_hex) == true);
}

TEST_CASE("msgpack can decode strings")
{
  SECTION("truncation_error")
  {
    REQUIRE_THROWS_AS(msgpack::decode("d9"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("da00"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("db000000"_hex), msgpack::truncation_error);

    REQUIRE_THROWS_AS(msgpack::decode("a1"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("d901"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("d9026c"_hex), msgpack::truncation_error);
  }

  REQUIRE(msgpack::decode("a0"_hex) == "");
  REQUIRE(msgpack::decode("d90000"_hex) == "");
  REQUIRE(msgpack::decode("da000000"_hex) == "");
  REQUIRE(msgpack::decode("db0000000000"_hex) == "");
  REQUIRE(msgpack::decode("a56c6f72656d"_hex) == "lorem");
  REQUIRE(msgpack::decode("d9056c6f72656d"_hex) == "lorem");
  REQUIRE(msgpack::decode("da00056c6f72656d"_hex) == "lorem");
  REQUIRE(msgpack::decode("db000000056c6f72656d"_hex) == "lorem");

  SECTION("leftovers")
  {
    auto const bytes =
        "a0a3616263d905697073756dda0005646f6c6f72db0000000873697420616d6574d90164"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(msgpack::decode(leftovers) == "");
    REQUIRE(msgpack::decode(leftovers) == "abc");
    REQUIRE(msgpack::decode(leftovers) == "ipsum");
    REQUIRE(msgpack::decode(leftovers) == "dolor");
    REQUIRE(msgpack::decode(leftovers) == "sit amet");
    REQUIRE(std::size(leftovers) == 3);
  }
}

TEST_CASE("msgpack can decode byte_stream")
{
  SECTION("truncation_error")
  {
    REQUIRE_THROWS_AS(msgpack::decode("c4"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("c401"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("c500"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("c50001"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("c6000000"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("c600000001"_hex), msgpack::truncation_error);
  }

  REQUIRE(msgpack::decode("c400"_hex) == ""_hex);
  REQUIRE(msgpack::decode("c50000"_hex) == ""_hex);
  REQUIRE(msgpack::decode("c600000000"_hex) == ""_hex);

  REQUIRE(msgpack::decode("c403010203"_hex) == "010203"_hex);
  REQUIRE(msgpack::decode("c50003010203"_hex) == "010203"_hex);
  REQUIRE(msgpack::decode("c600000003010203"_hex) == "010203"_hex);

  SECTION("leftovers")
  {
    auto const bytes = "c400c40101c500020203c600000003040506c40107"_hex;
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(msgpack::decode(leftovers) == ""_hex);
    REQUIRE(msgpack::decode(leftovers) == "01"_hex);
    REQUIRE(msgpack::decode(leftovers) == "0203"_hex);
    REQUIRE(msgpack::decode(leftovers) == "040506"_hex);
    REQUIRE(std::size(leftovers) == 3);
  }
}

TEST_CASE("msgpack can decode array")
{
  SECTION("truncation_error")
  {
    REQUIRE_THROWS_AS(msgpack::decode("91"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dc"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dc00"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dc0001"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dd"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dd00"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dd0000"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dd000000"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("dd00000001"_hex), msgpack::truncation_error);
  }
  SECTION("nested truncation_error")
  {
    REQUIRE_THROWS_AS(msgpack::decode("91c4"_hex), msgpack::truncation_error);
    REQUIRE_THROWS_AS(msgpack::decode("9300c4"_hex), msgpack::truncation_error);
  }
  SECTION("empty array")
  {
    cxx::json::array const array;
    REQUIRE(msgpack::decode("90"_hex) == array);
    REQUIRE(msgpack::decode("dc0000"_hex) == array);
    REQUIRE(msgpack::decode("dd00000000"_hex) == array);
  }
  SECTION("non-empty array")
  {
    cxx::json::array const a = {0};
    cxx::json::array const b = {1};
    cxx::json::array const c = {0x2a};
    REQUIRE(msgpack::decode("9100"_hex) == a);
    REQUIRE(msgpack::decode("dc000101"_hex) == b);
    REQUIRE(msgpack::decode("dd000000012a"_hex) == c);
    REQUIRE(msgpack::decode("9201a56c6f72656d"_hex) == cxx::json::array({1, "lorem"}));
  }
  SECTION("nested arrays")
  {
    auto const json = msgpack::decode("9190"_hex);
    REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
    REQUIRE(std::size(json) == 1);
    REQUIRE(json[0] == cxx::json::array());
    REQUIRE(msgpack::decode("929091a56c6f72656d"_hex) ==
            cxx::json::array({cxx::json::array(), cxx::json::array({"lorem"})}));
  }
  SECTION("leftovers")
  {
    auto const bytes = "909300a56c6f72656d2adc0001dd0000000101"_hex;
    auto const array = [] {
      cxx::json::array a, b;
      a.push_back(1);
      b.push_back(a);
      return b;
    }();
    cxx::json::byte_view leftovers(bytes.data(), std::size(bytes));
    REQUIRE(msgpack::decode(leftovers) == cxx::json::array());
    REQUIRE(msgpack::decode(leftovers) == cxx::json::array({0, "lorem", 0x2a}));
    REQUIRE(msgpack::decode(leftovers) == array);
    REQUIRE(std::size(leftovers) == 0);
  }
}
