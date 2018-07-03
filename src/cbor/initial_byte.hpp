#pragma once
#include "inc/cxx/json.hpp"

namespace cxx::codec::cbor
{
  template <typename T>
  constexpr auto base_type_impl()
  {
    if constexpr (std::is_enum_v<T>) { return std::underlying_type_t<T>{}; }
    else
    {
      return T{};
    }
  }

  template <typename T>
  using base_type = std::decay_t<decltype(base_type_impl<T>())>;

  struct initial_byte {
    struct type {
      static inline constexpr base_type<cxx::byte> positive = 0;
      static inline constexpr base_type<cxx::byte> negative = 1;
      static inline constexpr base_type<cxx::byte> bytes = 2;
      static inline constexpr base_type<cxx::byte> unicode = 3;
      static inline constexpr base_type<cxx::byte> array = 4;
      static inline constexpr base_type<cxx::byte> dictionary = 5;
      // static inline constexpr base_type<cxx::byte> tag = 6;
      static inline constexpr base_type<cxx::byte> special = 7;
    };
    struct value {
      static inline constexpr base_type<cxx::byte> max_insitu = 23;
      // static inline constexpr base_type<cxx::byte> one_byte = 24;
      // static inline constexpr base_type<cxx::byte> two_bytes = 25;
      // static inline constexpr base_type<cxx::byte> four_bytes = 26;
      static inline constexpr base_type<cxx::byte> eigth_bytes = 27;

      static inline constexpr base_type<cxx::byte> False = 0xf4;
      static inline constexpr base_type<cxx::byte> True = 0xf5;
      static inline constexpr base_type<cxx::byte> Null = 0xf6;
      static inline constexpr base_type<cxx::byte> ieee_754_double = 0xfb;
    };

    base_type<cxx::byte> additional : 5; // lower
    base_type<cxx::byte> major : 3;      // higher
  };
  static_assert(sizeof(initial_byte) == sizeof(cxx::byte));

  constexpr auto const initial = cxx::overload(
      [](cxx::json::byte_stream::reference byte) -> initial_byte* {
        return reinterpret_cast<initial_byte*>(&byte);
      },
      [](cxx::json::byte_stream::const_reference byte) -> initial_byte const* {
        return reinterpret_cast<initial_byte const*>(&byte);
      });
} // namespace cxx::codec::cbor
