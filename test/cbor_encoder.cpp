#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include <type_traits>
#include <limits>
using namespace cxx::literals;

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

namespace
{
  template <char h, char l, char... rest>
  void byte_literal(stream& bytes)
  {
    static_assert((h >= 'a' && h <= 'f') || (h >= '0' && h <= '9'));
    static_assert((l >= 'a' && l <= 'f') || (l >= '0' && l <= '9'));
    char h_base = 0;
    char l_base = 0;
    if
      constexpr(h >= 'a' && h <= 'f') { h_base = 'a' - 10; }
    else
    {
      h_base = '0';
    }
    if
      constexpr(l >= 'a' && l <= 'f') { l_base = 'a' - 10; }
    else
    {
      l_base = '0';
    }
    bytes.emplace_back((stream::value_type(h - h_base) << 4) | stream::value_type(l - l_base));
    if
      constexpr(sizeof...(rest) > 1) byte_literal<rest...>(bytes);
  }

  template <typename T, T... args>
  stream operator"" _hex()
  {
    static_assert(sizeof...(args) % 2 == 0);
    stream bytes;
    bytes.reserve(sizeof...(args) / 2);
    byte_literal<args...>(bytes);
    return bytes;
  }
}

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
    for (int i = 0; i < 0x18; i++) {
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
    for (int i = -1; i > -0x19; --i) {
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
  REQUIRE(cbor::encode(cxx::null) == "f6"_hex);
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
  REQUIRE(cbor::encode("lorem") == stream({0x65, 'l', 'o', 'r', 'e', 'm'}));
  REQUIRE(cbor::encode("ipsum dolor sit amet consectetur") ==
          stream({0x78, 0x20, 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o',
                  'r',  ' ',  's', 'i', 't', ' ', 'a', 'm', 'e', 't', ' ', 'c',
                  'o',  'n',  's', 'e', 'c', 't', 'e', 't', 'u', 'r'}));
}

TEST_CASE("cbor can encode double")
{
  REQUIRE(cbor::encode(0.0) == "fb0000000000000000"_hex);
  REQUIRE(cbor::encode(1.0) == "fb3ff0000000000000"_hex);
  REQUIRE(cbor::encode(1.1) == "fb3ff199999999999a"_hex);
}

TEST_CASE("cbor can encode arrays")
{
  REQUIRE(cbor::encode(cxx::array()) == "80"_hex);
  REQUIRE(cbor::encode({7}) == "8107"_hex);
  REQUIRE(cbor::encode({7, "lorem"}) == stream({0x82, 0x07, 0x65, 'l', 'o', 'r', 'e', 'm'}));
}

TEST_CASE("cbor can encode documents")
{
  REQUIRE(cbor::encode(cxx::document()) == "a0"_hex);
  REQUIRE(cbor::encode({{"lorem"_key, 42}, {"ipsum"_key, "dolor"}}) ==
          stream({0xa2, 0x65, 'i',  'p', 's', 'u', 'm', 0x65, 'd',  'o', 'l',
                  'o',  'r',  0x65, 'l', 'o', 'r', 'e', 'm',  0x18, 0x2a}));
}
