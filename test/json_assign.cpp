#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"
#include <type_traits>

using namespace test::literals;

TEST_CASE("cxx::json is copy assignable")
{
  static_assert(std::is_copy_assignable_v<cxx::json>);
  cxx::json const orig = 42;
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json != orig);

  json = orig;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == orig);
}

TEST_CASE("cxx::json is nothrow move assignable")
{
  static_assert(std::is_nothrow_move_assignable_v<cxx::json>);
  cxx::json orig = 42;
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json != orig);

  json = std::move(orig);
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == 42);
}

TEST_CASE("can nothrow assign std::int64_t to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, std::int64_t>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json != 42);

  json = 42l;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == 42);
}

TEST_CASE("can nothrow assign double to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, double>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<double>(json));
  REQUIRE(json != 3.14);

  json = 3.14;
  REQUIRE(cxx::holds_alternative<double>(json));
  REQUIRE(json == 3.14);
}

TEST_CASE("can nothrow assign cxx::json::null_t to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, cxx::json::null_t>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::null_t>(json));
  REQUIRE(json != cxx::json::null);

  json = cxx::json::null;
  REQUIRE(cxx::holds_alternative<cxx::json::null_t>(json));
  REQUIRE(json == cxx::json::null);
  REQUIRE_FALSE(json != cxx::json::null);
  REQUIRE_FALSE(cxx::json::null != cxx::json::null);
}

TEST_CASE("can nothrow assign bool to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, bool>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<bool>(json));
  REQUIRE(json != true);

  json = true;
  REQUIRE(cxx::holds_alternative<bool>(json));
  REQUIRE(json == true);
}

TEST_CASE("can assign std::string to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, std::string>);
  cxx::json json;
  std::string const lorem = "lorem";
  REQUIRE_FALSE(cxx::holds_alternative<std::string>(json));
  REQUIRE(json != lorem);

  json = lorem;
  REQUIRE(cxx::holds_alternative<std::string>(json));
  REQUIRE(json == lorem);
}

TEST_CASE("can nothrow assign std::string rvalue to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, std::string&&>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<std::string>(json));
  REQUIRE(json != "lorem");

  json = std::string("lorem");
  REQUIRE(cxx::holds_alternative<std::string>(json));
  REQUIRE(json == "lorem");
}

TEST_CASE("can assign json::byte_stream to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, cxx::json::byte_stream>);
  cxx::json json;
  auto const bytes = "deadbeef"_hex;
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::byte_stream>(json));
  REQUIRE(json != bytes);

  json = bytes;
  REQUIRE(cxx::holds_alternative<cxx::json::byte_stream>(json));
  REQUIRE(json == bytes);
}

TEST_CASE("can nothrow assign json::byte_stream rvalue to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, cxx::json::byte_stream&&>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::byte_stream>(json));
  REQUIRE(json != "deadbeef"_hex);

  json = "deadbeef"_hex;
  REQUIRE(cxx::holds_alternative<cxx::json::byte_stream>(json));
  REQUIRE(json == "deadbeef"_hex);
}

TEST_CASE("can assign cxx::json::array to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, cxx::json::array>);
  cxx::json json;
  cxx::json::array const array = {42, cxx::json::null, "lorem", true};
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json != array);

  json = array;
  REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json == array);
}

TEST_CASE("can nothrow assign cxx::json::array rvalue to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, cxx::json::array&&>);
  cxx::json json;
  cxx::json::array const array = {42, cxx::json::null, "lorem", true};
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json != array);

  cxx::json::array copy = array;
  json = std::move(copy);

  REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json == array);
}

TEST_CASE("can assign cxx::json::dictionary to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, cxx::json::dictionary>);
  cxx::json json = 42;
  cxx::json::dictionary const dict = {
      // clang-format off
      {"lorem", 42},
      {"ipsum", cxx::json::null},
      {"dolor", "sit"},
      {"amet", true}
      // clang-format on
  };
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::dictionary>(json));
  REQUIRE(json != dict);

  json = dict;
  REQUIRE(cxx::holds_alternative<cxx::json::dictionary>(json));
  REQUIRE(json == dict);
}

TEST_CASE("can nothrow assign cxx::json::dictionary rvalue to cxx::json")
{
  static_assert(std::is_nothrow_assignable_v<cxx::json, cxx::json::dictionary&&>);
  cxx::json json = 42;

  cxx::json::dictionary const dict = {
      // clang-format off
      {"lorem", 42},
      {"ipsum", cxx::json::null},
      {"dolor", "sit"},
      {"amet", true}
      // clang-format on
  };
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::dictionary>(json));
  REQUIRE(json != dict);

  cxx::json::dictionary copy = dict;
  json = std::move(copy);

  REQUIRE(cxx::holds_alternative<cxx::json::dictionary>(json));
  REQUIRE(json == dict);
}

TEST_CASE("can assign std::initializer_list<cxx::json> to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::json>>);
  cxx::json json;
  REQUIRE(cxx::holds_alternative<cxx::json::dictionary>(json));

  json = {42, true, cxx::json::null, 3.14};
  REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json == cxx::json::array({42, true, cxx::json::null, 3.14}));
  REQUIRE(std::size(json) == 4);
}

TEST_CASE("can assign std::initializer_list<cxx::byte> to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::byte>>);
  cxx::json json;
  REQUIRE_FALSE(cxx::holds_alternative<cxx::json::byte_stream>(json));

  json = {cxx::byte(0xde), cxx::byte(0xad), cxx::byte(0xbe), cxx::byte(0xef)};
  REQUIRE(cxx::holds_alternative<cxx::json::byte_stream>(json));
  REQUIRE(json == "deadbeef"_hex);
  REQUIRE(std::size(json) == 4);
}

TEST_CASE("can assign std::initializer_list<cxx::json::dictionary::value_type> to cxx::json")
{
  using namespace cxx::literals;
  static_assert(
      std::is_assignable_v<cxx::json, std::initializer_list<cxx::json::dictionary::value_type>>);
  cxx::json json = 42;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, "dolor"},
      {"sit"_key, cxx::json::null},
      {"amet"_key, 3.14},
      {"consectetur"_key, true}
      // clang-format on
  };
  REQUIRE(cxx::holds_alternative<cxx::json::dictionary>(json));
  cxx::json::dictionary const dictionary = {
      // clang-format off
      {"lorem", 42},
      {"ipsum", "dolor"},
      {"sit", cxx::json::null},
      {"amet", 3.14},
      {"consectetur", true}
      // clang-format on
  };

  REQUIRE(std::size(json) == std::size(dictionary));
  REQUIRE(json == dictionary);
}
