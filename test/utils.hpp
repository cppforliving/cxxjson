#pragma once
#include "inc/cxx/json.hpp"

namespace test::literals
{
  template <char h, char l, char... rest>
  void byte_literal(cxx::json::byte_stream& bytes)
  {
    static_assert((h >= 'a' && h <= 'f') || (h >= '0' && h <= '9'));
    static_assert((l >= 'a' && l <= 'f') || (l >= '0' && l <= '9'));
    char h_base = 0;
    char l_base = 0;
    if
      constexpr(h >= 'a' && h <= 'f') { h_base = 'a' - 10; }
    else
    {
      h_base = '0';
    }
    if
      constexpr(l >= 'a' && l <= 'f') { l_base = 'a' - 10; }
    else
    {
      l_base = '0';
    }
    bytes.emplace_back((cxx::json::byte_stream::value_type(h - h_base) << 4) |
                       cxx::json::byte_stream::value_type(l - l_base));
    if
      constexpr(sizeof...(rest) > 1) byte_literal<rest...>(bytes);
  }

  template <typename T, T... args>
  cxx::json::byte_stream operator"" _hex()
  {
    static_assert(sizeof...(args) % 2 == 0);
    cxx::json::byte_stream bytes;
    if
      constexpr(sizeof...(args) != 0)
      {
        bytes.reserve(sizeof...(args) / 2);
        byte_literal<args...>(bytes);
      }
    return bytes;
  }
}
