#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

TEST_CASE("cbor can encode arrays")
{
  REQUIRE(cbor::encode(cxx::array()) == stream({0x80}));
  REQUIRE(cbor::encode({7}) == stream({0x81, 0x07}));
  REQUIRE(cbor::encode({7, "lorem"}) == stream({0x82, 0x07, 0x45, 'l', 'o', 'r', 'e', 'm'}));
}
