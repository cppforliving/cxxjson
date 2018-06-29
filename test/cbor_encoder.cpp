#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"
#include <type_traits>
#include <limits>
using namespace cxx::literals;
using namespace test::literals;

using cbor = cxx::cbor;
using stream = cxx::json::byte_stream;

TEST_CASE("cbor sets correct initial byte for integers")
{
  SECTION("positive")
  {
    REQUIRE(cbor::encode(0x00).front() == cxx::byte(0x00));
    REQUIRE(cbor::encode(0x17).front() == cxx::byte(0x17));
    REQUIRE(cbor::encode(0x18).front() == cxx::byte(0x18));
    REQUIRE(cbor::encode(0xff).front() == cxx::byte(0x18));
    REQUIRE(cbor::encode(0x100).front() == cxx::byte(0x19));
    REQUIRE(cbor::encode(0xffff).front() == cxx::byte(0x19));
    REQUIRE(cbor::encode(0x10000).front() == cxx::byte(0x1a));
    REQUIRE(cbor::encode(0xffffffff).front() == cxx::byte(0x1a));
    REQUIRE(cbor::encode(0x100000000).front() == cxx::byte(0x1b));
    REQUIRE(cbor::encode(std::numeric_limits<std::int64_t>::max()).front() == cxx::byte(0x1b));
  }
  SECTION("negative")
  {
    REQUIRE(cbor::encode(-0x01).front() == cxx::byte(0x20));
    REQUIRE(cbor::encode(-0x18).front() == cxx::byte(0x37));
    REQUIRE(cbor::encode(-0x19).front() == cxx::byte(0x38));
    REQUIRE(cbor::encode(-0x100).front() == cxx::byte(0x38));
    REQUIRE(cbor::encode(-0x101).front() == cxx::byte(0x39));
    REQUIRE(cbor::encode(-0x10000).front() == cxx::byte(0x39));
    REQUIRE(cbor::encode(-0x10001).front() == cxx::byte(0x3a));
    REQUIRE(cbor::encode(-0x100000000).front() == cxx::byte(0x3a));
    REQUIRE(cbor::encode(-0x100000001).front() == cxx::byte(0x3b));
    REQUIRE(cbor::encode(std::numeric_limits<std::int64_t>::min()).front() == cxx::byte(0x3b));
  }
}

TEST_CASE("cbor can encode positive integers")
{
  SECTION("that fit on initial byte")
  {
    for (int i = 0; i < 0x18; i++)
    {
      auto const bytes = cbor::encode(i);
      REQUIRE(std::size(bytes) == 1);
      REQUIRE(bytes[0] == cxx::byte(i));
    }
  }
  SECTION("that fit on one additional byte")
  {
    REQUIRE(cbor::encode(0x18) == "1818"_hex);
    REQUIRE(cbor::encode(0xff) == "18ff"_hex);
  }
  SECTION("that fit on two additional bytes")
  {
    REQUIRE(cbor::encode(0x100) == "190100"_hex);
    REQUIRE(cbor::encode(0xfffe) == "19fffe"_hex);
  }
  SECTION("that fit on four additional bytes")
  {
    REQUIRE(cbor::encode(0x10000) == "1a00010000"_hex);
    REQUIRE(cbor::encode(0x12345678) == "1a12345678"_hex);
  }
  SECTION("that fir on eight additional bytes")
  {
    REQUIRE(cbor::encode(0x100000000) == "1b0000000100000000"_hex);
    REQUIRE(cbor::encode(0x1122334455667788) == "1b1122334455667788"_hex);
  }
}

TEST_CASE("cbor can encode negative integers")
{
  SECTION("that fit on initial byte")
  {
    for (int i = -1; i > -0x19; --i)
    {
      auto const bytes = cbor::encode(i);
      REQUIRE(std::size(bytes) == 1);
      REQUIRE(bytes[0] == cxx::byte((-i - 1) | 0b00100000));
    }
  }
  SECTION("that fit on one additional byte")
  {
    REQUIRE(cbor::encode(-0x19) == "3818"_hex);
    REQUIRE(cbor::encode(-0x100) == "38ff"_hex);
  }
  SECTION("that fit on two additional bytes")
  {
    REQUIRE(cbor::encode(-0x101) == "390100"_hex);
    REQUIRE(cbor::encode(-0xffff) == "39fffe"_hex);
  }
  SECTION("that fit on four additional bytes")
  {
    REQUIRE(cbor::encode(-0x10001) == "3a00010000"_hex);
    REQUIRE(cbor::encode(-0x12345679) == "3a12345678"_hex);
  }
  SECTION("that fir on eight additional bytes")
  {
    REQUIRE(cbor::encode(-0x100000001) == "3b0000000100000000"_hex);
    REQUIRE(cbor::encode(-0x1122334455667789) == "3b1122334455667788"_hex);
  }
}

TEST_CASE("cbor can encode null")
{
  REQUIRE(cbor::encode(cxx::json::null) == "f6"_hex);
}

TEST_CASE("cbor can encode booleans")
{
  REQUIRE(cbor::encode(false) == "f4"_hex);
  REQUIRE(cbor::encode(true) == "f5"_hex);
}

TEST_CASE("cbor can encode strings")
{
  REQUIRE(cbor::encode("") == "60"_hex);
  REQUIRE(cbor::encode("a") == "6161"_hex);
  REQUIRE(cbor::encode("IETF") == "6449455446"_hex);
  REQUIRE(cbor::encode("\"\\") == "62225c"_hex);
  REQUIRE(cbor::encode("ü") == "62c3bc"_hex);
  REQUIRE(cbor::encode("水") == "63e6b0b4"_hex);
  REQUIRE(cbor::encode("𐅑") == "64f0908591"_hex);
  REQUIRE(cbor::encode("lorem") == "656c6f72656d"_hex);
  REQUIRE(cbor::encode("ipsum dolor sit amet consectetur") ==
          "7820697073756d20646f6c6f722073697420616d657420636f6e7365637465747572"_hex);
}

TEST_CASE("cbor can encode byte_stream")
{
  REQUIRE(cbor::encode(""_hex) == "40"_hex);
  REQUIRE(cbor::encode("0a"_hex) == "410a"_hex);
  REQUIRE(cbor::encode("deadbeef"_hex) == "44deadbeef"_hex);
}

TEST_CASE("cbor can encode double")
{
  REQUIRE(cbor::encode(0.0) == "fb0000000000000000"_hex);
  REQUIRE(cbor::encode(1.0) == "fb3ff0000000000000"_hex);
  REQUIRE(cbor::encode(1.1) == "fb3ff199999999999a"_hex);
}

TEST_CASE("cbor can encode arrays")
{
  REQUIRE(cbor::encode(cxx::json::array()) == "80"_hex);
  REQUIRE(cbor::encode({7}) == "8107"_hex);
  REQUIRE(cbor::encode({7, "lorem"}) == "8207656c6f72656d"_hex);
}

TEST_CASE("cbor can encode documents")
{
  REQUIRE(cbor::encode(cxx::json::document()) == "a0"_hex);
  REQUIRE(cbor::encode({{"lorem"_key, 42}, {"ipsum"_key, "dolor"}}) ==
          "a265697073756d65646f6c6f72656c6f72656d182a"_hex);
}

TEST_CASE("official cbor test vectors")
{
  REQUIRE(cbor::encode(0) == "00"_hex);
  REQUIRE(cbor::encode(1) == "01"_hex);
  REQUIRE(cbor::encode(10) == "0a"_hex);
  REQUIRE(cbor::encode(23) == "17"_hex);
  REQUIRE(cbor::encode(24) == "1818"_hex);
  REQUIRE(cbor::encode(25) == "1819"_hex);
  REQUIRE(cbor::encode(100) == "1864"_hex);
  REQUIRE(cbor::encode(1000) == "1903e8"_hex);
  REQUIRE(cbor::encode(1000000) == "1a000f4240"_hex);
  REQUIRE(cbor::encode(1000000000000) == "1b000000e8d4a51000"_hex);
  // REQUIRE(cbor::encode(18446744073709551615) == "1bffffffffffff,
  // 0xffff"_hex);
  // error: integer constant is so large that it is unsigned
  // REQUIRE(cbor::encode(/*bignum*/18446744073709551616) ==
  //         "c249010000000000000000"_hex);
  // error: integer constant is too large for its type
  // REQUIRE(cbor::encode(-18446744073709551616) ==
  //         "3bffffffffffffffff"_hex);
  // error: integer constant is too large for its type
  // REQUIRE(cbor::encode(-18446744073709551617) ==
  //         "c349010000000000000000"_hex);
  // error: integer constant is too large for its type
  REQUIRE(cbor::encode(-1) == "20"_hex);
  REQUIRE(cbor::encode(-10) == "29"_hex);
  REQUIRE(cbor::encode(-100) == "3863"_hex);
  REQUIRE(cbor::encode(-1000) == "3903e7"_hex);
  // REQUIRE(cbor::encode(-0.0) == "f98000"_hex);
  // sizeof(double) == 8
  REQUIRE(cbor::encode(1.0e+300) == "fb7e37e43c8800759c"_hex);
  REQUIRE(cbor::encode(false) == "f4"_hex);
  REQUIRE(cbor::encode(true) == "f5"_hex);
  REQUIRE(cbor::encode(cxx::json::null) == "f6"_hex);
  REQUIRE(cbor::encode("") == "60"_hex);
  REQUIRE(cbor::encode("a") == "6161"_hex);
  REQUIRE(cbor::encode("IETF") == "6449455446"_hex);
  REQUIRE(cbor::encode("\"\\") == "62225c"_hex);
  REQUIRE(cbor::encode("ü") == "62c3bc"_hex);
  REQUIRE(cbor::encode("水") == "63e6b0b4"_hex);
  REQUIRE(cbor::encode("𐅑") == "64f0908591"_hex);
  REQUIRE(cbor::encode(cxx::json::array()) == "80"_hex);
  REQUIRE(cbor::encode({1, 2, 3}) == "83010203"_hex);
  REQUIRE(cbor::encode({1, {2, 3}, {4, 5}}) == "8301820203820405"_hex);
  REQUIRE(cbor::encode({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}) ==
          "98190102030405060708090a0b0c0d0e0f101112131415161718181819"_hex);
  REQUIRE(cbor::encode(cxx::json::document()) == "a0"_hex);
  REQUIRE(cbor::encode({"a"_key >> 1, {"b"_key, {2, 3}}}) == "a26161016162820203"_hex);
  REQUIRE(cbor::encode({"a", {"b"_key >> "c"}}) == "826161a161626163"_hex);
  REQUIRE(cbor::encode({"a"_key >> "A", "b"_key >> "B", "c"_key >> "C", "d"_key >> "D",
                        "e"_key >> "E"}) == "a56161614161626142616361436164614461656145"_hex);
}
