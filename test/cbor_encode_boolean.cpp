#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

TEST_CASE("cbor can encode booleans")
{
  REQUIRE(cbor::encode(false) == stream({0xf4}));
  REQUIRE(cbor::encode(true) == stream({0xf5}));
}
