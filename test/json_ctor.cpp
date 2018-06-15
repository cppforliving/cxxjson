#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>

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

TEST_CASE("can create cxx::json std::int64_t")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, std::int64_t>);
  std::int64_t const x = 42;
  cxx::json const json(x);
  REQUIRE(json == x);
  REQUIRE(json != 7);
  REQUIRE_FALSE(json != x);
}

TEST_CASE("can create cxx::json double")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, double>);
  double const x = 3.14;
  cxx::json const json(x);
  REQUIRE(json == x);
  REQUIRE(json != 7.0);
  REQUIRE_FALSE(json != x);
}
