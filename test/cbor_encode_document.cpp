#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;
using stream = cbor::byte_stream;
using namespace cxx::literals;

TEST_CASE("cbor can encode documents")
{
  REQUIRE(cbor::encode(cxx::document()) == stream({0xa0}));
  REQUIRE(cbor::encode({{"lorem"_key, 42}, {"ipsum"_key, "dolor"}}) ==
          stream({0xa2, 0x45, 'i',  'p', 's', 'u', 'm', 0x45, 'd',  'o', 'l',
                  'o',  'r',  0x45, 'l', 'o', 'r', 'e', 'm',  0x18, 0x2a}));
}
