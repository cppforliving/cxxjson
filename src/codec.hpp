#pragma once
#include "inc/cxx/json.hpp"
#include <arpa/inet.h>

namespace cxx
{
  struct codec {
    /**
     *
     */
    static constexpr std::size_t const max_nesting = 0x3f;

    /**
     *
     */
    static constexpr auto const sum = [](auto const&... x) -> decltype(auto) { return (x + ...); };

    /**
     *
     */
    static constexpr auto const code = [](std::uint64_t x) {
      return 0x3 & sum((x >> 32) != 0, (x >> 16) != 0, (x >> 8) != 0);
    };

    /**
     *
     */
    static constexpr auto const space = [](std::uint64_t x) { return 1u << code(x); };

    /**
     *
     */
    static constexpr auto const available = [](cxx::json::byte_stream const& stream) {
      return stream.capacity() - std::size(stream);
    };

    /**
     *
     */
    static constexpr auto const append = [](cxx::json::byte_stream& stream,
                                            cxx::json::byte_stream::size_type const size) {
      return stream.reserve(stream.capacity() + size);
    };

    /**
     *
     */
    static constexpr auto const assure = [](cxx::json::byte_stream& stream,
                                            cxx::json::byte_stream::size_type const needed) {
      if (available(stream) < needed) append(stream, needed);
    };

    /**
     *
     */
    static constexpr auto const htonll = [](std::uint64_t x) -> std::uint64_t {
      return (static_cast<std::uint64_t>(htonl(static_cast<std::uint32_t>(x & 0xffffffff))) << 32) |
             htonl(static_cast<std::uint32_t>(x >> 32));
    };

    /**
     *
     */
    static constexpr auto const ntohll = [](std::uint64_t x) -> std::uint64_t {
      auto const hi = static_cast<std::uint32_t>(x >> 32);
      auto const lo = static_cast<std::uint32_t>(x & 0xffffffff);
      return (static_cast<std::uint64_t>(ntohl(lo)) << 32) | ntohl(hi);
    };

    /**
     *
     */
    static constexpr auto const htond = [](double d) -> double {
      static_assert(sizeof(double) == sizeof(std::uint64_t));
      static_assert(alignof(double) == alignof(std::uint64_t));
      auto* pu = static_cast<std::uint64_t*>(static_cast<void*>(&d));
      *pu = htonll(*pu);
      return d;
    };

    /**
     *
     */
    static constexpr auto const ntohf = [](float f) -> float {
      static_assert(sizeof(float) == sizeof(std::uint32_t));
      static_assert(alignof(float) == alignof(std::uint32_t));
      auto* pu = static_cast<std::uint32_t*>(static_cast<void*>(&f));
      *pu = ntohl(*pu);
      return f;
    };

    /**
     *
     */
    static constexpr auto const htonf = [](float f) -> float {
      static_assert(sizeof(float) == sizeof(std::uint32_t));
      static_assert(alignof(float) == alignof(std::uint32_t));
      auto* pu = static_cast<std::uint32_t*>(static_cast<void*>(&f));
      *pu = htonl(*pu);
      return f;
    };

    /**
     *
     */
    static constexpr auto const ntohd = [](double d) -> double {
      static_assert(sizeof(double) == sizeof(std::uint64_t));
      static_assert(alignof(double) == alignof(std::uint64_t));
      auto* pu = static_cast<std::uint64_t*>(static_cast<void*>(&d));
      *pu = ntohll(*pu);
      return d;
    };

    /**
     *
     */
    static constexpr auto const ntoh =
        cxx::overload{[](std::uint8_t x) -> std::uint8_t { return x; },
                      [](std::uint16_t x) -> std::uint16_t { return ntohs(x); },
                      [](std::uint32_t x) -> std::uint32_t { return ntohl(x); },
                      [](std::uint64_t x) -> std::uint64_t { return ntohll(x); },
                      [](float x) -> float { return ntohf(x); },
                      [](double x) -> double { return ntohd(x); }};

    /**
     *
     */
    static constexpr auto const hton =
        cxx::overload{[](std::uint8_t x) -> std::uint8_t { return x; },
                      [](std::uint16_t x) -> std::uint16_t { return htons(x); },
                      [](std::uint32_t x) -> std::uint32_t { return htonl(x); },
                      [](std::uint64_t x) -> std::uint64_t { return htonll(x); },
                      [](float x) -> float { return htonf(x); },
                      [](double x) -> double { return htond(x); }};

    /**
     *
     */
    static constexpr auto const read_from = [](auto& t, cxx::json::byte_view bytes) {
      auto* dest = static_cast<cxx::byte*>(static_cast<void*>(&t));
      std::copy(bytes.data(), bytes.data() + sizeof(t), dest);
    };

    /**
     *
     */
    static constexpr auto const write_to = [](auto const& t, cxx::json::byte_stream::pointer dest) {
      auto const* first =
          static_cast<cxx::json::byte_stream::const_pointer>(static_cast<void const*>(&t));
      std::copy(first, first + sizeof(t), dest);
    };

    /**
     *
     */
    template <typename T>
    static constexpr auto const base_type_impl = [] {
      if constexpr (std::is_enum_v<T>) { return std::underlying_type_t<T>{}; }
      else
      {
        return T{};
      }
    };

    /**
     *
     */
    template <typename T>
    using base_type = std::decay_t<decltype(base_type_impl<T>())>;

    /*
     *
     */
    using numbyte = base_type<cxx::byte>;

    /**
     *
     */
    template <typename T>
    static constexpr auto const reserved = [](typename T::size_type capa) -> T {
      T ret;
      ret.reserve(capa);
      return ret;
    };

    template <std::size_t S>
    static constexpr auto const nbtoh = [](cxx::json::byte_view bytes) -> std::uint64_t {
      if constexpr (S == sizeof(std::uint8_t)) { return static_cast<std::uint64_t>(bytes.front()); }
      else if constexpr (S == sizeof(std::uint16_t))
      {
        std::uint16_t x;
        read_from(x, bytes);
        return ntoh(x);
      }
      else if constexpr (S == sizeof(std::uint32_t))
      {
        std::uint32_t x;
        read_from(x, bytes);
        return ntoh(x);
      }
      else if constexpr (S == sizeof(std::uint64_t))
      {
        std::uint64_t x;
        read_from(x, bytes);
        return ntoh(x);
      }
    };
  };
} // namespace cxx
