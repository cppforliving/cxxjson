#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;
using stream = cbor::byte_stream;

TEST_CASE("cbor can encode strings")
{
  REQUIRE(cbor::encode("lorem") == stream({0x45, 'l', 'o', 'r', 'e', 'm'}));
  REQUIRE(cbor::encode("ipsum dolor sit amet consectetur") ==
          stream({0x58, 0x20, 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o',
                  'r',  ' ',  's', 'i', 't', ' ', 'a', 'm', 'e', 't', ' ', 'c',
                  'o',  'n',  's', 'e', 'c', 't', 'e', 't', 'u', 'r'}));
}
