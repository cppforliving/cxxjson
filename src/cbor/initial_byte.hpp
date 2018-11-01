#pragma once
#include "inc/cxx/json.hpp"
#include "src/codec.hpp"
#include <arpa/inet.h>

namespace cxx::detail::cbor
{
  struct initial_byte {
    struct type {
      static inline constexpr ::cxx::codec::numbyte positive = 0;
      static inline constexpr ::cxx::codec::numbyte negative = 1;
      static inline constexpr ::cxx::codec::numbyte bytes = 2;
      static inline constexpr ::cxx::codec::numbyte unicode = 3;
      static inline constexpr ::cxx::codec::numbyte array = 4;
      static inline constexpr ::cxx::codec::numbyte dictionary = 5;
      // static inline constexpr ::cxx::codec::numbyte tag = 6;
      static inline constexpr ::cxx::codec::numbyte special = 7;
    };
    struct value {
      static inline constexpr ::cxx::codec::numbyte max_insitu = 23;
      static inline constexpr ::cxx::codec::numbyte one_byte = 24;
      static inline constexpr ::cxx::codec::numbyte two_bytes = 25;
      static inline constexpr ::cxx::codec::numbyte four_bytes = 26;
      static inline constexpr ::cxx::codec::numbyte eigth_bytes = 27;
      static inline constexpr ::cxx::codec::numbyte indefinite = 0x1f;

      static inline constexpr ::cxx::codec::numbyte False = 0xf4;
      static inline constexpr ::cxx::codec::numbyte True = 0xf5;
      static inline constexpr ::cxx::codec::numbyte Null = 0xf6;
      static inline constexpr ::cxx::codec::numbyte Simple = 0xf8;
      static inline constexpr ::cxx::codec::numbyte ieee_754_half = 0xf9;
      static inline constexpr ::cxx::codec::numbyte ieee_754_single = 0xfa;
      static inline constexpr ::cxx::codec::numbyte ieee_754_double = 0xfb;
    };

    ::cxx::codec::numbyte additional : 5; // lower
    ::cxx::codec::numbyte major : 3;      // higher
  };
  static_assert(sizeof(initial_byte) == sizeof(cxx::byte));

  constexpr auto const initial =
      cxx::overload{[](cxx::json::byte_stream::reference byte) -> initial_byte* {
                      return reinterpret_cast<initial_byte*>(&byte);
                    },
                    [](cxx::json::byte_stream::const_reference byte) -> initial_byte const* {
                      return reinterpret_cast<initial_byte const*>(&byte);
                    }};
} // namespace cxx::detail::cbor
