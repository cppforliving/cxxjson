#include "inc/cxx/json.hpp"
#include "test/catch.hpp"
#include <type_traits>
#include <memory>

TEST_CASE("cxx::get can use const cxx::json")
{
  cxx::json const json;
  REQUIRE(cxx::holds_alternative<cxx::json::document>(json));
  REQUIRE_THROWS_AS(cxx::get<cxx::json::array>(json), std::bad_variant_access);

  auto const& document = cxx::get<cxx::json::document>(json);
  REQUIRE(json == document);
}

TEST_CASE("cxx::get can use non-const cxx::json")
{
  cxx::json json;
  REQUIRE(cxx::holds_alternative<cxx::json::document>(json));
  REQUIRE_THROWS_AS(cxx::get<cxx::json::array>(json), std::bad_variant_access);

  auto& document = cxx::get<cxx::json::document>(json);
  document["lorem"] = 42l;
  REQUIRE(json == document);
}

TEST_CASE("const cxx::json allows item access for string keys")
{
  using namespace cxx::literals;
  SECTION("throws exception if underlying object is not a document")
  {
    cxx::json const json = 42;
    REQUIRE_THROWS_AS(json["sit"], std::bad_variant_access);
  }
  cxx::json const json = {
      // clang-format off
      "lorem"_key >> 42,
      "ipsum"_key >> cxx::json::null,
      "dolor"_key >> "sit",
      "amet"_key >> 3.14
      // clang-format on
  };
  SECTION("throws exception for nonexisting key")
  {
    REQUIRE_THROWS_AS(json["sit"], std::out_of_range);
  }
  REQUIRE(json["lorem"] == 42);
  REQUIRE(json["ipsum"] == cxx::json::null);
  REQUIRE(json["dolor"] == "sit");
  REQUIRE(json["amet"] == 3.14);
}

TEST_CASE("non-const cxx::json allows item access for string keys")
{
  using namespace cxx::literals;
  SECTION("throws exception if underlying object is not a document")
  {
    cxx::json json = 42;
    REQUIRE_THROWS_AS(json["sit"], std::bad_variant_access);
  }
  cxx::json json = {
      // clang-format off
      "lorem"_key >> 42,
      "ipsum"_key >> cxx::json::null,
      "dolor"_key >> "sit",
      "amet"_key >> 3.14
      // clang-format on
  };
  REQUIRE(json["lorem"] == 42);
  REQUIRE(json["ipsum"] == cxx::json::null);
  REQUIRE(json["dolor"] == "sit");
  REQUIRE(json["amet"] == 3.14);

  SECTION("returns new default created cxx::json object")
  {
    auto& nyu = json["sit"];
    REQUIRE(cxx::holds_alternative<cxx::json::document>(nyu));
    REQUIRE(std::size(nyu) == 0);
    REQUIRE(std::empty(nyu));
    nyu["consectetur"] = 7;
    REQUIRE(json["sit"]["consectetur"] == 7);
  }
}

TEST_CASE("const cxx::json allows item access for int keys")
{
  using namespace cxx::literals;
  SECTION("throws exception if underlying object is not a array")
  {
    cxx::json const json = 42;
    REQUIRE_THROWS_AS(json[42], std::bad_variant_access);
  }
  cxx::json const json = {42, cxx::json::null, "lorem", 3.14};

  SECTION("throws exception for nonexisting key")
  {
    REQUIRE_THROWS_AS(json[42], std::out_of_range);
  }
  REQUIRE(json[0] == 42);
  REQUIRE(json[1] == cxx::json::null);
  REQUIRE(json[2] == "lorem");
  REQUIRE(json[3] == 3.14);
}

TEST_CASE("non-const cxx::json allows item access for int keys")
{
  using namespace cxx::literals;
  SECTION("throws exception if underlying object is not a array")
  {
    cxx::json json = 42;
    REQUIRE_THROWS_AS(json[42], std::bad_variant_access);
  }
  cxx::json json = {42, cxx::json::null, "lorem", 3.14};

  SECTION("throws exception for nonexisting key")
  {
    REQUIRE_THROWS_AS(json[42], std::out_of_range);
  }
  REQUIRE(json[0] == 42);
  REQUIRE(json[1] == cxx::json::null);
  REQUIRE(json[2] == "lorem");
  REQUIRE(json[3] == 3.14);
}
