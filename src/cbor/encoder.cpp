#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"
#include <algorithm>
#include <arpa/inet.h>

namespace
{
  auto const sum = [](auto const&... x) -> decltype(auto) { return (x + ...); };

  std::uint64_t htonll(std::uint64_t x) noexcept
  {
    return (static_cast<std::uint64_t>(htonl(static_cast<std::uint32_t>(x & 0xffffffff))) << 32) |
           htonl(static_cast<std::uint32_t>(x >> 32));
  }

  auto const available = [](cxx::json::byte_stream const& stream) {
    return stream.capacity() - std::size(stream);
  };

  auto const append = [](cxx::json::byte_stream& stream,
                         cxx::json::byte_stream::size_type const space) {
    return stream.reserve(stream.capacity() + space);
  };

  auto const assure = [](cxx::json::byte_stream& stream,
                         cxx::json::byte_stream::size_type const needed) {
    if (available(stream) < needed) append(stream, needed);
  };
} // namespace

namespace detail
{
  void encode(cxx::json const&, cxx::json::byte_stream&) noexcept;

  cxx::byte& encode_positive_integer(std::uint64_t x, cxx::json::byte_stream& stream) noexcept
  {
    auto& init = stream.emplace_back(cxx::byte(x));
    if (x <= ::cxx::codec::cbor::initial_byte::value::max_insitu) return init;
    auto const code = 0x3 & sum((x >> 32) != 0, (x >> 16) != 0, (x >> 8) != 0);
    init = cxx::byte(::cxx::codec::cbor::initial_byte::value::max_insitu + code + 1);
    auto const space = (1u << code);
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

  void encode_negative_integer(std::int64_t x, cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::cbor::initial(
        encode_positive_integer(static_cast<std::uint64_t>(-(x + 1)), stream))
        ->major = ::cxx::codec::cbor::initial_byte::type::negative;
  }

  void encode(std::int64_t x, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, sizeof(std::int64_t) + 1);
    if (x < 0) return encode_negative_integer(x, stream);
    encode_positive_integer(static_cast<std::uint64_t>(x), stream);
  }

  void encode(cxx::json::byte_stream const& x, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, std::size(x) + sizeof(std::uint64_t) + 1);
    ::cxx::codec::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::codec::cbor::initial_byte::type::bytes;
    stream.insert(std::end(stream), std::begin(x), std::end(x));
  }

  void encode(std::string const& x, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, std::size(x) + sizeof(std::uint64_t) + 1);
    ::cxx::codec::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::codec::cbor::initial_byte::type::unicode;
    auto first = reinterpret_cast<cxx::byte const*>(x.data());
    stream.insert(std::end(stream), first, first + std::size(x));
  }

  void encode(cxx::json::array const& x, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, sizeof(std::uint64_t) + 1 + std::size(x) * sizeof(cxx::json));
    ::cxx::codec::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::codec::cbor::initial_byte::type::array;
    for (auto const& item : x) ::detail::encode(item, stream);
  }

  void encode(cxx::json::dictionary const& x, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, sizeof(std::uint64_t) + 1 + 2 * std::size(x) * sizeof(cxx::json));
    ::cxx::codec::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::codec::cbor::initial_byte::type::dictionary;
    for (auto const& [key, value] : x)
    {
      ::detail::encode(key, stream);
      ::detail::encode(value, stream);
    }
  }

  void encode(bool b, cxx::json::byte_stream& stream) noexcept
  {
    stream.emplace_back(cxx::byte(b ? ::cxx::codec::cbor::initial_byte::value::True
                                    : ::cxx::codec::cbor::initial_byte::value::False));
  }

  void encode(cxx::json::null_t, cxx::json::byte_stream& stream) noexcept
  {
    stream.emplace_back(cxx::byte(::cxx::codec::cbor::initial_byte::value::Null));
  }

  void encode(double d, cxx::json::byte_stream& stream) noexcept
  {
    assure(stream, sizeof(double) + 1);
    stream.emplace_back(cxx::byte(::cxx::codec::cbor::initial_byte::value::ieee_754_double));
    auto dest = stream.insert(std::end(stream), sizeof(double), cxx::byte());
    auto const* first = reinterpret_cast<cxx::byte const*>(&d);
    std::reverse_copy(first, first + sizeof(double), dest);
  }

  void encode(cxx::json const& json, cxx::json::byte_stream& stream) noexcept
  {
    cxx::visit([&stream](auto const& x) { ::detail::encode(x, stream); }, json);
  }
} // namespace detail

auto ::cxx::cbor::encode(json const& j) noexcept -> json::byte_stream
{
  json::byte_stream stream;
  ::detail::encode(j, stream);
  return stream;
}
