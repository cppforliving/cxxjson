#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>

TEST_CASE("can create cxx::json from std::initializer_list<cxx::json>")
{
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::json>>);
  cxx::json json;
  REQUIRE(cxx::holds_alternative<cxx::document>(json));

  json = {42, true, cxx::null, 3.14};
  REQUIRE(cxx::holds_alternative<cxx::array>(json));
  REQUIRE(json == cxx::array({42, true, cxx::null, 3.14}));
  REQUIRE(std::size(json) == 4);
}

TEST_CASE("can create cxx::json from std::initializer_list<cxx::document::value_type>")
{
  static_assert(std::is_assignable_v<cxx::json, std::initializer_list<cxx::document::value_type>>);
  cxx::json json = 42;
  REQUIRE(cxx::holds_alternative<std::int64_t>(json));
  json = {
      // clang-format off
      {"lorem", 42},
      {"ipsum", "dolor"},
      {"sit", cxx::null},
      {"amet", 3.14},
      {"consectetur", true}
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
