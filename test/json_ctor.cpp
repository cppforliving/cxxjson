#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>

using namespace std::string_literals;

TEST_CASE("can default construct cxx::json")
{
  static_assert(std::is_nothrow_default_constructible_v<cxx::json>);
  cxx::json json;
}

TEST_CASE("can copy construct cxx::json")
{
  static_assert(std::is_copy_constructible_v<cxx::json>);
  cxx::json const orig;
  cxx::json copy(orig);
  REQUIRE(orig == copy);
}

TEST_CASE("can move construct cxx::json")
{
  static_assert(std::is_nothrow_move_constructible_v<cxx::json>);
  cxx::json orig;
  cxx::json copy(std::move(orig));
}

TEST_CASE("can create cxx::json from std::int64_t")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, std::int64_t>);
  std::int64_t const x = 42;
  cxx::json const json(x);
  REQUIRE(json == x);
}

TEST_CASE("can create cxx::json from double")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, double>);
  double const x = 3.14;
  cxx::json const json(x);
  REQUIRE(json == x);
}

TEST_CASE("can create cxx::json from bool")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, bool>);
  cxx::json const json(true);
  REQUIRE(json == true);
}

TEST_CASE("can create cxx::json from cxx::null")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::null_t>);
  cxx::json const json(cxx::null);
  REQUIRE(json == cxx::null);
}

TEST_CASE("can create cxx::json from std::string")
{
  static_assert(std::is_constructible_v<cxx::json, std::string const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, std::string&&>);
  std::string const lorem = "lorem";
  cxx::json const json(lorem);
  REQUIRE(json == lorem);
  cxx::json const ipsum(std::string("ipsum"));
  REQUIRE(ipsum == std::string("ipsum"));
}

TEST_CASE("can create cxx::json from cxx::array")
{
  static_assert(std::is_constructible_v<cxx::json, cxx::array const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::array&&>);
  cxx::array const array = {{true, cxx::null, 42l, 3.14}};
  cxx::json const json(array);
  REQUIRE(json == array);
  cxx::json const arr(cxx::array({cxx::null, 42l}));
  REQUIRE(arr == cxx::array({cxx::null, 42l}));
}

TEST_CASE("can create cxx::json from cxx::document")
{
  static_assert(std::is_constructible_v<cxx::json, cxx::document const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::document&&>);
  cxx::document const document = {
      // clang-format off
      {"lorem"s, 42l},
      {"ipsum"s, cxx::null},
      {"dolor"s, true},
      {"sit"s, 3.14}
      // clang-format on
  };
  cxx::json const json(document);
  REQUIRE(json == document);
  cxx::json const doc(cxx::document({{"lorem"s, cxx::null}, {"ipsum"s, 3.14}}));
  REQUIRE(doc == cxx::document({{"lorem"s, cxx::null}, {"ipsum"s, 3.14}}));
}
