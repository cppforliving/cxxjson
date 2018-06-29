#include "inc/cxx/cbor.hpp"
#include "test/catch.hpp"

using cbor = cxx::cbor;

TEST_CASE("cbor transcoding of integers")
{
  auto const assert_transcoding = [](std::int64_t x) {
    REQUIRE(cbor::decode(cbor::encode(x)) == x);
  };
  assert_transcoding(0x00);
  assert_transcoding(0x17);
  assert_transcoding(0x18);
  assert_transcoding(0xff);
  assert_transcoding(0x100);
  assert_transcoding(0xffff);
  assert_transcoding(0x10000);
  assert_transcoding(0xffffffff);
  assert_transcoding(0x100000000);
  assert_transcoding(-0x01);
  assert_transcoding(-0x18);
  assert_transcoding(-0x19);
  assert_transcoding(-0x100);
  assert_transcoding(-0x101);
  assert_transcoding(-0x10000);
  assert_transcoding(-0x10001);
  assert_transcoding(-0x100000000);
  assert_transcoding(-0x100000001);
}
