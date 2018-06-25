#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"
#include <type_traits>
#include <limits>
using namespace cxx::literals;

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

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
    REQUIRE(cbor::encode(0x18) == stream({0x18, 0x18}));
    REQUIRE(cbor::encode(0xff) == stream({0x18, 0xff}));
  }
  SECTION("that fit on two additional bytes")
  {
    REQUIRE(cbor::encode(0x100) == stream({0x19, 0x01, 0x00}));
    REQUIRE(cbor::encode(0xfffe) == stream({0x19, 0xff, 0xfe}));
  }
  SECTION("that fit on four additional bytes")
  {
    REQUIRE(cbor::encode(0x10000) == stream({0x1a, 0x00, 0x01, 0x00, 0x00}));
    REQUIRE(cbor::encode(0x12345678) == stream({0x1a, 0x12, 0x34, 0x56, 0x78}));
  }
  SECTION("that fir on eight additional bytes")
  {
    REQUIRE(cbor::encode(0x100000000) ==
            stream({0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}));
    REQUIRE(cbor::encode(0x1122334455667788) ==
            stream({0x1b, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}));
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
    REQUIRE(cbor::encode(-0x19) == stream({0x38, 0x18}));
    REQUIRE(cbor::encode(-0x100) == stream({0x38, 0xff}));
  }
  SECTION("that fit on two additional bytes")
  {
    REQUIRE(cbor::encode(-0x101) == stream({0x39, 0x01, 0x00}));
    REQUIRE(cbor::encode(-0xffff) == stream({0x39, 0xff, 0xfe}));
  }
  SECTION("that fit on four additional bytes")
  {
    REQUIRE(cbor::encode(-0x10001) == stream({0x3a, 0x00, 0x01, 0x00, 0x00}));
    REQUIRE(cbor::encode(-0x12345679) == stream({0x3a, 0x12, 0x34, 0x56, 0x78}));
  }
  SECTION("that fir on eight additional bytes")
  {
    REQUIRE(cbor::encode(-0x100000001) ==
            stream({0x3b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}));
    REQUIRE(cbor::encode(-0x1122334455667789) ==
            stream({0x3b, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}));
  }
}

TEST_CASE("cbor can encode null")
{
  REQUIRE(cbor::encode(cxx::null) == stream({0xf6}));
}

TEST_CASE("cbor can encode booleans")
{
  REQUIRE(cbor::encode(false) == stream({0xf4}));
  REQUIRE(cbor::encode(true) == stream({0xf5}));
}

TEST_CASE("cbor can encode strings")
{
  REQUIRE(cbor::encode("lorem") == stream({0x45, 'l', 'o', 'r', 'e', 'm'}));
  REQUIRE(cbor::encode("ipsum dolor sit amet consectetur") ==
          stream({0x58, 0x20, 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o',
                  'r',  ' ',  's', 'i', 't', ' ', 'a', 'm', 'e', 't', ' ', 'c',
                  'o',  'n',  's', 'e', 'c', 't', 'e', 't', 'u', 'r'}));
}

TEST_CASE("cbor can encode double")
{
  REQUIRE(cbor::encode(0.0) == stream({0xfb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
  REQUIRE(cbor::encode(1.0) == stream({0xfb, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
  REQUIRE(cbor::encode(1.1) == stream({0xfb, 0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a}));
}

TEST_CASE("cbor can encode arrays")
{
  REQUIRE(cbor::encode(cxx::array()) == stream({0x80}));
  REQUIRE(cbor::encode({7}) == stream({0x81, 0x07}));
  REQUIRE(cbor::encode({7, "lorem"}) == stream({0x82, 0x07, 0x45, 'l', 'o', 'r', 'e', 'm'}));
}

TEST_CASE("cbor can encode documents")
{
  REQUIRE(cbor::encode(cxx::document()) == stream({0xa0}));
  REQUIRE(cbor::encode({{"lorem"_key, 42}, {"ipsum"_key, "dolor"}}) ==
          stream({0xa2, 0x45, 'i',  'p', 's', 'u', 'm', 0x45, 'd',  'o', 'l',
                  'o',  'r',  0x45, 'l', 'o', 'r', 'e', 'm',  0x18, 0x2a}));
}
