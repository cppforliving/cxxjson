#include "inc/cxx/cbor.hpp"
#include <arpa/inet.h>

namespace
{
  struct initial_byte {
    static constexpr cxx::byte max_insitu = 23;
    struct type {
      static constexpr cxx::byte positive = 0;
      static constexpr cxx::byte negative = 1;
    };
    cxx::byte additional : 5; // lower
    cxx::byte major : 3;      // higher
  };
  static_assert(sizeof(initial_byte) == sizeof(cxx::byte));

  constexpr auto const initial = cxx::overload(
      [](cxx::cbor::byte_stream::reference byte) -> initial_byte* {
        return reinterpret_cast<initial_byte*>(&byte);
      },
      [](cxx::cbor::byte_stream::const_reference byte) -> initial_byte const* {
        return reinterpret_cast<initial_byte const*>(&byte);
      });

  auto const sum = [](auto const&... x) -> decltype(auto) { return (x + ...); };

  std::uint64_t htonll(std::uint64_t x)
  {
    return (static_cast<std::uint64_t>(htonl(static_cast<std::uint32_t>(x & 0xffffffff))) << 32) |
           htonl(static_cast<std::uint32_t>(x >> 32));
  }
}

namespace detail
{
  cxx::byte& encode_positive_integer(std::int64_t x, cxx::cbor::byte_stream& stream)
  {
    auto& init = stream.emplace_back(cxx::byte(x));
    if (x <= initial_byte::max_insitu) return init;
    auto const code = sum((x >> 32) != 0, (x >> 16) != 0, (x >> 8) != 0);
    init = cxx::byte(initial_byte::max_insitu + code + 1);
    auto const space = (1 << code);
    auto it = stream.insert(std::end(stream), space, {});
    switch (code)
    {
      case 0:
        *it = cxx::byte(0xff & x);
        break;
      case 1:
        reinterpret_cast<std::uint16_t&>(*it) = htons(static_cast<std::uint16_t>(0xffff & x));
        break;
      case 2:
        reinterpret_cast<std::uint32_t&>(*it) = htonl(static_cast<std::uint32_t>(0xffffffff & x));
        break;
      case 3:
        reinterpret_cast<std::uint64_t&>(*it) = htonll(x);
        break;
    }
    return *(--it);
  }

  cxx::byte& encode_negative_integer(std::int64_t x, cxx::cbor::byte_stream& stream)
  {
    auto& data = encode_positive_integer(-x - 1, stream);
    initial(data)->major = initial_byte::type::negative;
    return data;
  }

  cxx::byte& encode(std::int64_t x, cxx::cbor::byte_stream& stream)
  {
    if (x < 0) return encode_negative_integer(x, stream);
    return encode_positive_integer(x, stream);
  }

  template <typename T>
  cxx::byte encode(T const&, cxx::cbor::byte_stream&)
  {
    return {};
  }
}

auto ::cxx::cbor::encode(json const& j) noexcept -> byte_stream
{
  byte_stream stream;
  auto const func = [&stream](auto const& x) { detail::encode(x, stream); };
  cxx::visit(func, j);
  return stream;
}
