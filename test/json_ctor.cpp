#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>

using namespace std::string_literals;

TEST_CASE("can default construct cxx::json")
{
  static_assert(std::is_nothrow_default_constructible_v<cxx::json>);
  cxx::json const json;
  REQUIRE(cxx::holds_alternative<cxx::json::document>(json));
  REQUIRE(std::size(json) == 0);
}

TEST_CASE("can copy construct cxx::json")
{
  static_assert(std::is_copy_constructible_v<cxx::json>);
  cxx::json const orig;
  cxx::json const copy(orig);
  REQUIRE(cxx::holds_alternative<cxx::json::document>(orig));
  REQUIRE(cxx::holds_alternative<cxx::json::document>(copy));
  REQUIRE(orig == copy);
}

TEST_CASE("can move construct cxx::json")
{
  static_assert(std::is_nothrow_move_constructible_v<cxx::json>);
  cxx::json orig;
  cxx::json const copy(std::move(orig));
  REQUIRE(cxx::holds_alternative<cxx::json::document>(copy));
}

TEST_CASE("can directly initialize cxx::json from std::int64_t")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, std::int64_t>);
  std::int64_t const x = 42;
  cxx::json const json(x);
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == x);
  REQUIRE(std::size(json) == 1);
}

TEST_CASE("can copy initialize cxx::json from std::int64_t")
{
  cxx::json const json = 42l;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == 42l);
}

TEST_CASE("can create cxx::json from double")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, double>);
  double const x = 3.14;
  cxx::json const json(x);
  REQUIRE(cxx::holds_alternative<double>(json));
  REQUIRE(json == x);
  REQUIRE(std::size(json) == 1);
}

TEST_CASE("can create cxx::json from bool")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, bool>);
  cxx::json const json(true);
  REQUIRE(cxx::holds_alternative<bool>(json));
  REQUIRE(json == true);
  REQUIRE(std::size(json) == 1);
}

TEST_CASE("can create cxx::json from cxx::json::null")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::json::null_t>);
  cxx::json const json(cxx::json::null);
  REQUIRE(cxx::holds_alternative<cxx::json::null_t>(json));
  REQUIRE(json == cxx::json::null);
  REQUIRE(std::size(json) == 0);
}

TEST_CASE("can create cxx::json from std::string")
{
  static_assert(std::is_constructible_v<cxx::json, std::string const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, std::string&&>);
  std::string const lorem = "lorem";
  cxx::json const json(lorem);
  REQUIRE(cxx::holds_alternative<std::string>(json));
  REQUIRE(json == lorem);
  REQUIRE(std::size(lorem) == 5);

  cxx::json const ipsum(std::string("ipsum"));
  REQUIRE(cxx::holds_alternative<std::string>(ipsum));
  REQUIRE(ipsum == std::string("ipsum"));
  REQUIRE(std::size(ipsum) == 5);
}

TEST_CASE("can create cxx::json from cxx::json::array")
{
  static_assert(std::is_constructible_v<cxx::json, cxx::json::array const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::json::array&&>);
  cxx::json::array const array = {true, cxx::json::null, 42l, 3.14};
  cxx::json const json(array);
  REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json == array);
  REQUIRE(std::size(json) == 4);

  cxx::json const arr(cxx::json::array({cxx::json::null, 42l}));
  REQUIRE(arr == cxx::json::array({cxx::json::null, 42l}));
  REQUIRE(std::size(arr) == 2);
}

TEST_CASE("can create cxx::json from cxx::json::document")
{
  static_assert(std::is_constructible_v<cxx::json, cxx::json::document const&>);
  static_assert(std::is_nothrow_constructible_v<cxx::json, cxx::json::document&&>);
  cxx::json::document const document = {
      // clang-format off
      {"lorem"s, 42l},
      {"ipsum"s, cxx::json::null},
      {"dolor"s, true},
      {"sit"s, 3.14}
      // clang-format on
  };
  cxx::json const json(document);
  REQUIRE(cxx::holds_alternative<cxx::json::document>(json));
  REQUIRE(json == document);
  REQUIRE(std::size(json) == 4);

  cxx::json const doc(cxx::json::document({{"lorem"s, cxx::json::null}, {"ipsum"s, 3.14}}));
  REQUIRE(cxx::holds_alternative<cxx::json::document>(doc));
  REQUIRE(doc == cxx::json::document({{"lorem"s, cxx::json::null}, {"ipsum"s, 3.14}}));
  REQUIRE(std::size(doc) == 2);
}

TEST_CASE("can create cxx::json from std::initializer_list<json>")
{
  static_assert(std::is_constructible_v<cxx::json, std::initializer_list<cxx::json>>);
  cxx::json const json = {42, true, cxx::json::null, 3.14};

  REQUIRE(cxx::holds_alternative<cxx::json::array>(json));
  REQUIRE(json == cxx::json::array({42, true, cxx::json::null, 3.14}));
}

TEST_CASE("can create cxx::json from std::initializer_list<std::pair<json::key, json>>")
{
  using namespace cxx::literals;
  static_assert(
      std::is_constructible_v<cxx::json,
                              std::initializer_list<std::pair<cxx::json::key const, cxx::json>>>);
  cxx::json const json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, true},
      {"dolor"_key, cxx::json::null},
      {"sit"_key, 3.14}
      // clang-format on
  };

  REQUIRE(cxx::holds_alternative<cxx::json::document>(json));
  REQUIRE(json ==
          // clang-format off
          cxx::json::document(
            {
              {"lorem", 42},
              {"ipsum", true},
              {"dolor", cxx::json::null},
              {"sit", 3.14}
            }
          )
          // clang-format on
          );
}

TEST_CASE("can directly initialize cxx::json from int")
{
  static_assert(std::is_nothrow_constructible_v<cxx::json, int>);
  int const x = 42;
  cxx::json const json(x);
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == x);
}

TEST_CASE("can copy initialize cxx::json from int")
{
  cxx::json const json = 42;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  REQUIRE(json == 42);
}
