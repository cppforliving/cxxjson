#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>

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

TEST_CASE("can assign std::initializer_list<cxx::json> to cxx::json")
{
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::json>>);
  cxx::json json;
  REQUIRE(cxx::holds_alternative<cxx::document>(json));

  json = {42, true, cxx::null, 3.14};
  REQUIRE(cxx::holds_alternative<cxx::array>(json));
  REQUIRE(json == cxx::array({42, true, cxx::null, 3.14}));
  REQUIRE(std::size(json) == 4);
}

TEST_CASE("can assign std::initializer_list<cxx::document::value_type> to cxx::json")
{
  using namespace cxx::literals;
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::document::value_type>>);
  cxx::json json = 42;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, "dolor"},
      {"sit"_key, cxx::null},
      {"amet"_key, 3.14},
      {"consectetur"_key, true}
      // clang-format on
  };
  REQUIRE(cxx::holds_alternative<cxx::document>(json));
  cxx::document const document = {
      // clang-format off
      {"lorem", 42},
      {"ipsum", "dolor"},
      {"sit", cxx::null},
      {"amet", 3.14},
      {"consectetur", true}
      // clang-format on
  };

  REQUIRE(std::size(json) == std::size(document));
  REQUIRE(json == document);
}
