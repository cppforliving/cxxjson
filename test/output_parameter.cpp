#include "inc/cxx/by_ref.hpp"
#include "test/catch.hpp"
#include <type_traits>

struct object {
  int value;
};

using param = cxx::output_parameter<object>;

TEST_CASE("can construct cxx::output_parameter")
{
  static_assert(!std::is_constructible_v<param, object const&>);
  static_assert(!std::is_constructible_v<param, object>);
  static_assert(!std::is_constructible_v<param, object&&>);
  static_assert(!std::is_default_constructible_v<param>);
  static_assert(!std::is_copy_constructible_v<param>);
  static_assert(!std::is_copy_assignable_v<param>);
  static_assert(std::is_assignable_v<param, object>);
  object obj = {42};
  object const other = {13};
  param ref(obj);
  REQUIRE(std::addressof(obj) == std::addressof(*ref));
  ref->value = 7;
  REQUIRE(obj.value == 7);
  ref = other;
  REQUIRE(obj.value == 13);
}

TEST_CASE("can pass cxx::output_parameter")
{
  object obj = {42};
  [](cxx::output_parameter<object> x) {
    return [](cxx::output_parameter<object> y) { y->value = 7; }(cxx::by_ref(x));
  }(cxx::by_ref(obj));
  REQUIRE(obj.value == 7);
}
