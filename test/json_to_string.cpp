#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include "test/utils.hpp"

using namespace test::literals;

TEST_CASE("can serialize simple values to string")
{
  using namespace cxx::literals;
  REQUIRE(cxx::to_string(cxx::json::null) == "null");
  REQUIRE(cxx::to_string(42) == "42");
  REQUIRE(cxx::to_string(3.14) == "3.140000");
  REQUIRE(cxx::to_string(true) == "true");
  REQUIRE(cxx::to_string(false) == "false");
  REQUIRE(cxx::to_string("lorem \"ipsum\" dolor") == "\"lorem \\\"ipsum\\\" dolor\"");
  REQUIRE_THROWS_AS(cxx::to_string("616263"_hex), std::invalid_argument);
  REQUIRE(cxx::to_string(cxx::json::array()) == "[]");
  REQUIRE(cxx::to_string({42, true, cxx::json::null, "lorem"}) == "[42, true, null, \"lorem\"]");
  REQUIRE(cxx::to_string(cxx::json::dictionary()) == "{}");
  REQUIRE(cxx::to_string({"lorem"_key >> 42, "ipsum"_key >> true, "dolor"_key >> cxx::json::null,
                          "sit"_key >> "amet"}) ==
          "{\"dolor\": null, \"ipsum\": true, \"lorem\": 42, \"sit\": \"amet\"}");
}
