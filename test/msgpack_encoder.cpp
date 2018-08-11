#include "inc/cxx/msgpack.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"
#include "inc/cxx/json.hpp"

using namespace cxx::literals;
using namespace test::literals;

using msgpack = cxx::msgpack;

TEST_CASE("msgpack can encode simple special values")
{
  REQUIRE(msgpack::encode(cxx::json::null) == "c0"_hex);
  REQUIRE(msgpack::encode(false) == "c2"_hex);
  REQUIRE(msgpack::encode(true) == "c3"_hex);
}

TEST_CASE("msgpack can encode integers")
{
  REQUIRE(msgpack::encode(0) == "00"_hex);
  REQUIRE(msgpack::encode(0x1f) == "1f"_hex);
  REQUIRE(msgpack::encode(0x7f) == "7f"_hex);
  REQUIRE(msgpack::encode(0x8f) == "cc8f"_hex);
  REQUIRE(msgpack::encode(0xff) == "ccff"_hex);
  REQUIRE(msgpack::encode(0x100) == "cd0100"_hex);
  REQUIRE(msgpack::encode(0xffff) == "cdffff"_hex);
  REQUIRE(msgpack::encode(0x10000) == "ce00010000"_hex);
  REQUIRE(msgpack::encode(0xffffffff) == "ceffffffff"_hex);
  REQUIRE(msgpack::encode(0x100000000) == "cf0000000100000000"_hex);
  REQUIRE(msgpack::encode(0x7fffffffffffffff) == "cf7fffffffffffffff"_hex);
  REQUIRE(msgpack::encode(-0x01) == "ff"_hex);
  REQUIRE(msgpack::encode(-0x20) == "e0"_hex);
  REQUIRE(msgpack::encode(-0x21) == "d0df"_hex);
  REQUIRE(msgpack::encode(-0x80) == "d080"_hex);
  REQUIRE(msgpack::encode(-0x100) == "d1ff00"_hex);
  REQUIRE(msgpack::encode(-1000) == "d1fc18"_hex);
  REQUIRE(msgpack::encode(-0x8000) == "d18000"_hex);
  REQUIRE(msgpack::encode(-0x10000) == "d2ffff0000"_hex);
  REQUIRE(msgpack::encode(-0x80000000l) == "d280000000"_hex);
  REQUIRE(msgpack::encode(-0x1000000000000000l) == "d3f000000000000000"_hex);
}

TEST_CASE("msgpack can encode strings")
{
  REQUIRE(msgpack::encode("lorem") == "d9056c6f72656d"_hex);
  REQUIRE(msgpack::encode("lorem ipsum dolor sit amet consectetur") ==
          "d9266c6f72656d20697073756d20646f6c6f722073697420616d657420636f6e7365637465747572"_hex);
}

TEST_CASE("msgpack can encode byte_stream")
{
  REQUIRE(msgpack::encode(""_hex) == "c400"_hex);
  REQUIRE(msgpack::encode("0001020304"_hex) == "c4050001020304"_hex);
}
