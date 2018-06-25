#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

TEST_CASE("cbor can encode null")
{
  REQUIRE(cbor::encode(cxx::null) == stream({0xf6}));
}
