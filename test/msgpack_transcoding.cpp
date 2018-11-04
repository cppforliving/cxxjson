#include "inc/cxx/msgpack.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace std::string_literals;
using namespace cxx::literals;
using namespace test::literals;

auto const transcode = [](auto const& x) -> cxx::json {
  using msgpack = cxx::msgpack;
  return msgpack::decode(msgpack::encode(x));
};

auto const assert_transcoding = [](auto const& x) { REQUIRE(transcode(x) == x); };

TEST_CASE("msgpack can transcode integers")
{
  assert_transcoding(0x00);
  assert_transcoding(0x7f);
  assert_transcoding(0x80);
  assert_transcoding(0xffffffff);
  assert_transcoding(std::numeric_limits<std::int64_t>::max());

  assert_transcoding(-0x01);
  assert_transcoding(-0x11);
  assert_transcoding(-0x20);
  assert_transcoding(-0x7001);
  assert_transcoding(-0x8000);
  assert_transcoding(std::numeric_limits<std::int64_t>::min());
}

TEST_CASE("msgpack can transcode simple values")
{
  assert_transcoding(cxx::json::null);
  assert_transcoding(true);
  assert_transcoding(false);
}

TEST_CASE("msgpack can transcode strings")
{
  assert_transcoding("");
  assert_transcoding("ü");
  assert_transcoding("水");
  assert_transcoding("𐅑");
  assert_transcoding(
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt "
      "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
      "ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
      "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur "
      "sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est "
      "laborum.");
}

TEST_CASE("msgpack can transcode bytes")
{
  assert_transcoding(""_hex);
  assert_transcoding("010203"_hex);
}

TEST_CASE("msgpack can transcode arrays")
{
  assert_transcoding(cxx::json::array());
  assert_transcoding(cxx::json::array({{0}}));
  assert_transcoding(cxx::json::array({0, "lorem"}));
  assert_transcoding(cxx::json::array({0, "lorem", cxx::json::array({0x2a, "ipsum"}), "dolor"}));
}

TEST_CASE("msgpack can transcode dictionaries")
{
  assert_transcoding(cxx::json::dictionary());
  assert_transcoding(cxx::json{{"x"_key >> 42}});
  assert_transcoding(cxx::json{
      // clang-format off
    {
      "lorem"_key >> 42,
      "ipsum"_key >> true,
      "dolor"_key >> cxx::json::null,
      {"sit"_key, {false, "lorem", 7}},
      {"amet"_key,
        {
          "x"_key >> 11,
          "y"_key >> cxx::json::null
        }
      }
    }
      // clang-format on
  });
}
