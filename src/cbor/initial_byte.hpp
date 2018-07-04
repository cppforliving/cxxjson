#pragma once
#include "inc/cxx/json.hpp"
#include <arpa/inet.h>

namespace cxx
{
  auto const htonll = [](std::uint64_t x) -> std::uint64_t {
    return (static_cast<std::uint64_t>(htonl(static_cast<std::uint32_t>(x & 0xffffffff))) << 32) |
           htonl(static_cast<std::uint32_t>(x >> 32));
  };

  auto const ntohll = [](std::uint64_t x) -> std::uint64_t {
    auto const hi = static_cast<std::uint32_t>(x >> 32);
    auto const lo = static_cast<std::uint32_t>(x & 0xffffffff);
    return (static_cast<std::uint64_t>(ntohl(lo)) << 32) | ntohl(hi);
  };

  auto const htond = [](double d) -> double {
    static_assert(sizeof(double) == sizeof(std::uint64_t));
    static_assert(alignof(double) == alignof(std::uint64_t));
    auto* pu = static_cast<std::uint64_t*>(static_cast<void*>(&d));
    *pu = htonll(*pu);
    return d;
  };

  auto const ntohf = [](float f) -> float {
    static_assert(sizeof(float) == sizeof(std::uint32_t));
    static_assert(alignof(float) == alignof(std::uint32_t));
    auto* pu = static_cast<std::uint32_t*>(static_cast<void*>(&f));
    *pu = ntohl(*pu);
    return f;
  };

  auto const ntohd = [](double d) -> double {
    static_assert(sizeof(double) == sizeof(std::uint64_t));
    static_assert(alignof(double) == alignof(std::uint64_t));
    auto* pu = static_cast<std::uint64_t*>(static_cast<void*>(&d));
    *pu = ntohll(*pu);
    return d;
  };

  auto const ntoh = cxx::overload([](std::uint16_t x) -> std::uint16_t { return ntohs(x); },
                                  [](std::uint32_t x) -> std::uint32_t { return ntohl(x); },
                                  [](std::uint64_t x) -> std::uint64_t { return ntohll(x); },
                                  [](float x) -> float { return ntohf(x); },
                                  [](double x) -> double { return ntohd(x); });
} // namespace cxx

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
      static inline constexpr base_type<cxx::byte> one_byte = 24;
      static inline constexpr base_type<cxx::byte> two_bytes = 25;
      static inline constexpr base_type<cxx::byte> four_bytes = 26;
      static inline constexpr base_type<cxx::byte> eigth_bytes = 27;
      static inline constexpr base_type<cxx::byte> infinite = 0x1f;

      static inline constexpr base_type<cxx::byte> False = 0xf4;
      static inline constexpr base_type<cxx::byte> True = 0xf5;
      static inline constexpr base_type<cxx::byte> Null = 0xf6;
      static inline constexpr base_type<cxx::byte> Simple = 0xf8;
      static inline constexpr base_type<cxx::byte> ieee_754_single = 0xfa;
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
