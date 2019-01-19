#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"
#include "src/codec.hpp"
#include <algorithm>
#include <arpa/inet.h>

namespace detail
{
  void encode(cxx::json const&, cxx::json::byte_stream&) noexcept;

  [[gnu::flatten]] cxx::byte& encode_positive_integer(std::uint64_t x,
                                                      cxx::json::byte_stream& stream) noexcept
  {
    auto& init = stream.emplace_back(cxx::byte(x));
    if (x <= ::cxx::detail::cbor::initial_byte::value::max_insitu) return init;
    auto const code = ::cxx::codec::code(x);
    init = cxx::byte(::cxx::detail::cbor::initial_byte::value::max_insitu + code + 1);
    auto const space = ::cxx::codec::space(x);
    auto it = stream.insert(std::end(stream), space, {});
    switch (code)
    {
      case 0:
        *it = cxx::byte(0xff & x);
        break;
      case 1:
        ::cxx::codec::write_to(::cxx::codec::hton(static_cast<std::uint16_t>(0xffff & x)),
                               std::addressof(*it));
        break;
      case 2:
        ::cxx::codec::write_to(::cxx::codec::hton(static_cast<std::uint32_t>(0xffffffff & x)),
                               std::addressof(*it));
        break;
      case 3:
        ::cxx::codec::write_to(::cxx::codec::hton(x), std::addressof(*it));
        break;
    }
    return *(--it);
  }

  [[gnu::flatten]] void encode_negative_integer(std::int64_t x,
                                                cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::detail::cbor::initial(
        encode_positive_integer(static_cast<std::uint64_t>(-(x + 1)), stream))
        ->major = ::cxx::detail::cbor::initial_byte::type::negative;
  }

  [[gnu::flatten]] void encode(std::int64_t x, cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream), sizeof(std::int64_t) + 1);
    if (x < 0) return encode_negative_integer(x, stream);
    encode_positive_integer(static_cast<std::uint64_t>(x), stream);
  }

  [[gnu::flatten]] void encode(cxx::json::byte_stream const& x,
                               cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream), std::size(x) + sizeof(std::uint64_t) + 1);
    ::cxx::detail::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::detail::cbor::initial_byte::type::bytes;
    stream.insert(std::end(stream), std::begin(x), std::end(x));
  }

  [[gnu::flatten]] void encode(std::string const& x, cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream), std::size(x) + sizeof(std::uint64_t) + 1);
    ::cxx::detail::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::detail::cbor::initial_byte::type::unicode;
    auto first = reinterpret_cast<cxx::byte const*>(x.data());
    stream.insert(std::end(stream), first, first + std::size(x));
  }

  [[gnu::flatten]] void encode(cxx::json::array const& x, cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream),
                         sizeof(std::uint64_t) + 1 + std::size(x) * sizeof(cxx::json));
    ::cxx::detail::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::detail::cbor::initial_byte::type::array;
    for (auto const& item : x) ::detail::encode(item, stream);
  }

  [[gnu::flatten]] void encode(cxx::json::dictionary const& x,
                               cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream),
                         sizeof(std::uint64_t) + 1 + 2 * std::size(x) * sizeof(cxx::json));
    ::cxx::detail::cbor::initial(encode_positive_integer(std::size(x), stream))->major =
        ::cxx::detail::cbor::initial_byte::type::dictionary;
    for (auto const& [key, value] : x)
    {
      ::detail::encode(key, stream);
      ::detail::encode(value, stream);
    }
  }

  [[gnu::flatten]] void encode(bool b, cxx::json::byte_stream& stream) noexcept
  {
    stream.emplace_back(cxx::byte(b ? ::cxx::detail::cbor::initial_byte::value::True
                                    : ::cxx::detail::cbor::initial_byte::value::False));
  }

  [[gnu::flatten]] void encode(cxx::json::null_t, cxx::json::byte_stream& stream) noexcept
  {
    stream.emplace_back(cxx::byte(::cxx::detail::cbor::initial_byte::value::Null));
  }

  [[gnu::flatten]] void encode(double d, cxx::json::byte_stream& stream) noexcept
  {
    ::cxx::codec::assure(cxx::by_ref(stream), sizeof(double) + 1);
    d = ::cxx::codec::hton(d);
    auto const* first = static_cast<cxx::byte const*>(static_cast<void const*>(&d));
    stream.emplace_back(cxx::byte(::cxx::detail::cbor::initial_byte::value::ieee_754_double));
    stream.insert(std::end(stream), first, first + sizeof(double));
  }

  [[gnu::noinline, gnu::flatten]] void encode(cxx::json const& json,
                                              cxx::json::byte_stream& stream) noexcept
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
