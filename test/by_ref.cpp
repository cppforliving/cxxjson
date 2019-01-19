#include "inc/cxx/by_ref.hpp"
#include "test/catch.hpp"
#include <type_traits>

struct object {
  int value;
};

using param = cxx::by_ref<object>;

TEST_CASE("can construct cxx::by_ref")
{
  static_assert(!std::is_constructible_v<param, object const&>);
  static_assert(!std::is_constructible_v<param, object>);
  static_assert(!std::is_constructible_v<param, object&&>);
  static_assert(!std::is_default_constructible_v<param>);
  static_assert(std::is_copy_constructible_v<param>);
  static_assert(!std::is_copy_assignable_v<param>);
  static_assert(std::is_assignable_v<param, object>);
  object obj = {42};
  object const other = {13};
  param ref(obj);
  ref->value = 7;
  REQUIRE(obj.value == 7);
  ref = other;
  REQUIRE(obj.value == 13);
}

TEST_CASE("can pass cxx::by_ref")
{
  object obj = {42};
  [](cxx::by_ref<object> x) {
    return [](cxx::by_ref<object> y) { y->value = 7; }(cxx::by_ref(x));
  }(cxx::by_ref(obj));
  REQUIRE(obj.value == 7);
}
