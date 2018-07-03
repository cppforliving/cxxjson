#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"
#include <algorithm>
#include <arpa/inet.h>

namespace
{
  using initial_byte = cxx::codec::cbor::initial_byte;
  template <cxx::codec::cbor::base_type<cxx::byte> t>
  using tag_t = std::integral_constant<cxx::codec::cbor::base_type<cxx::byte>, t>;
  template <cxx::codec::cbor::base_type<cxx::byte> t>
  constexpr tag_t<t> tag{};

  constexpr auto const ntoh = cxx::overload(
      [](std::uint8_t, cxx::cbor::byte_view bytes) -> std::int64_t {
        return static_cast<std::int64_t>(bytes.front());
      },
      [](std::uint16_t, cxx::cbor::byte_view bytes) -> std::int64_t {
        return ntohs(*reinterpret_cast<std::uint16_t const*>(bytes.data()));
      },
      [](std::uint32_t, cxx::cbor::byte_view bytes) -> std::int64_t {
        return ntohl(*reinterpret_cast<std::uint32_t const*>(bytes.data()));
      },
      [](std::uint64_t x, cxx::cbor::byte_view bytes) -> std::int64_t {
        auto const* pu = static_cast<std::uint64_t const*>(static_cast<void const*>(bytes.data()));
        x = cxx::ntohll(*pu);
        if (x > std::numeric_limits<std::int64_t>::max())
          throw cxx::cbor::unsupported("integer value bigger than std::int64_t max");
        return static_cast<std::int64_t>(x);
      });

  template <typename Sink>
  cxx::cbor::byte_view parse(cxx::cbor::byte_view, Sink);

  template <typename Int, typename Sink>
  cxx::cbor::byte_view parse(Int, cxx::cbor::byte_view bytes, Sink sink)
  {
    if (std::size(bytes) < sizeof(Int))
      throw cxx::cbor::buffer_error("not enough data to decode json");
    sink(ntoh(Int{}, bytes));
    bytes.remove_prefix(sizeof(Int));
    return bytes;
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::positive>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    auto const additional = cxx::codec::cbor::initial(byte)->additional;
    switch (additional)
    {
      case 0x00:
      case 0x01:
      case 0x02:
      case 0x03:
      case 0x04:
      case 0x05:
      case 0x06:
      case 0x07:
      case 0x08:
      case 0x09:
      case 0x0a:
      case 0x0b:
      case 0x0c:
      case 0x0d:
      case 0x0e:
      case 0x0f:
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
        sink(static_cast<std::int64_t>(additional));
        return bytes;
      case 0x18:
        return parse(std::uint8_t{0}, bytes, sink);
      case 0x19:
        return parse(std::uint16_t{0}, bytes, sink);
      case 0x1a:
        return parse(std::uint32_t{0}, bytes, sink);
      case 0x1b:
        return parse(std::uint64_t{0}, bytes, sink);
      default:
        throw cxx::cbor::data_error("meaningless additional data in initial byte");
    }
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::negative>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    return parse(tag<initial_byte::type::positive>, byte, bytes,
                 [&sink](std::int64_t x) { sink(-(x + 1)); });
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::bytes>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    std::size_t const size = [&] {
      std::size_t n = 0;
      auto const leftovers = parse(tag<initial_byte::type::positive>, byte, bytes,
                                   [&n](std::int64_t x) { n = static_cast<std::size_t>(x); });
      if (std::size(leftovers) < n)
        throw cxx::cbor::buffer_error("not enough data to decode byte_stream");
      bytes = leftovers;
      return n;
    }();
    sink(cxx::cbor::byte_view(bytes.data(), size));
    bytes.remove_prefix(size);
    return bytes;
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::unicode>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    return parse(tag<initial_byte::type::bytes>, byte, bytes, [&sink](cxx::cbor::byte_view x) {
      sink(std::string_view(reinterpret_cast<std::string_view::const_pointer>(x.data()),
                            std::size(x)));
    });
  }

  auto const emplace_to = [](auto& target, std::string_view key = std::string_view()) {
    using target_t = std::decay_t<decltype(target)>;
    using key_t = cxx::json::dictionary::key_type;
    auto impl = [&target, key](auto&& x) {
      (void)key;
      if constexpr (std::is_same_v<target_t, cxx::json::array>)
      { target.emplace_back(std::forward<decltype(x)>(x)); }
      else if constexpr (std::is_same_v<target_t, cxx::json::dictionary>)
      {
        target.try_emplace(key_t(key), std::forward<decltype(x)>(x));
      }
      else if constexpr (std::is_same_v<target_t, cxx::json>)
      {
        target = std::forward<decltype(x)>(x);
      }
      else
      {
        throw 1;
      }
    };
    return cxx::overload(
        [impl](cxx::cbor::byte_view bytes) {
          impl(cxx::json::byte_stream(bytes.data(), bytes.data() + std::size(bytes)));
        },
        impl);
  };

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::array>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    cxx::json::array::size_type size = 0;
    bytes = parse(tag<initial_byte::type::positive>, byte, bytes,
                  [&size](std::int64_t x) { size = static_cast<cxx::json::array::size_type>(x); });
    if (size > cxx::cbor::max_size)
      throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
    cxx::json::array array;
    array.reserve(size);
    while (size--) bytes = parse(bytes, emplace_to(array));
    sink(std::move(array));
    return bytes;
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::dictionary>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    cxx::json::dictionary::size_type size = 0;
    bytes = parse(tag<initial_byte::type::positive>, byte, bytes, [&size](std::int64_t x) {
      size = static_cast<cxx::json::dictionary::size_type>(x);
    });
    if (size > cxx::cbor::max_size)
      throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
    cxx::json::dictionary dict;
    while (size--)
    {
      if (std::empty(bytes))
        throw cxx::cbor::buffer_error("not enough data to decode dictionary key");
      auto const init = bytes.front();
      bytes.remove_prefix(1);
      std::string_view key;
      bytes = parse(tag<initial_byte::type::unicode>, init, bytes,
                    [&key](std::string_view x) { key = x; });
      bytes = parse(bytes, emplace_to(dict, key));
    }
    sink(std::move(dict));
    return bytes;
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(tag_t<initial_byte::type::special>,
                             cxx::byte byte,
                             cxx::cbor::byte_view bytes,
                             Sink sink)
  {
    auto const simple_value = [sink](cxx::byte v) {
      auto const x = static_cast<cxx::codec::cbor::base_type<cxx::byte>>(v);
      switch (x)
      {
        case initial_byte::value::False:
          sink(false);
          break;
        case initial_byte::value::True:
          sink(true);
          break;
        case initial_byte::value::Null:
          sink(cxx::json::null);
          break;
        default:
          throw cxx::cbor::unsupported("decoding given type is not yet supported");
      }
    };
    auto const floating_value = [sink](cxx::cbor::byte_view b) {
      auto const* pd = static_cast<double const*>(static_cast<void const*>(b.data()));
      sink(cxx::ntohd(*pd));
    };
    auto const value = static_cast<cxx::codec::cbor::base_type<cxx::byte>>(byte);
    switch (value)
    {
      case initial_byte::value::False:
        sink(false);
        break;
      case initial_byte::value::True:
        sink(true);
        break;
      case initial_byte::value::Null:
        sink(cxx::json::null);
        break;
      case 0xf8:
        if (std::empty(bytes))
          throw cxx::cbor::buffer_error("not enough data to decode simple value");
        simple_value(bytes.at(0));
        bytes.remove_prefix(1);
        break;
      case initial_byte::value::ieee_754_double:
        if (std::size(bytes) < sizeof(double))
          throw cxx::cbor::buffer_error("not enough data to decode double");
        floating_value(bytes);
        bytes.remove_prefix(sizeof(double));
        break;
      default:
        throw cxx::cbor::unsupported("decoding given type is not yet supported");
    }
    return bytes;
  }

  template <typename Sink>
  cxx::cbor::byte_view parse(cxx::cbor::byte_view bytes, Sink sink)
  {
    if (bytes.empty()) throw cxx::cbor::buffer_error("not enough data to decode json");
    auto const byte = bytes.front();
    bytes.remove_prefix(1);
    switch (cxx::codec::cbor::initial(byte)->major)
    {
      case initial_byte::type::positive:
        return parse(tag<initial_byte::type::positive>, byte, bytes, sink);
      case initial_byte::type::negative:
        return parse(tag<initial_byte::type::negative>, byte, bytes, sink);
      case initial_byte::type::bytes:
        return parse(tag<initial_byte::type::bytes>, byte, bytes, sink);
      case initial_byte::type::unicode:
        return parse(tag<initial_byte::type::unicode>, byte, bytes, sink);
      case initial_byte::type::array:
        return parse(tag<initial_byte::type::array>, byte, bytes, sink);
      case initial_byte::type::dictionary:
        return parse(tag<initial_byte::type::dictionary>, byte, bytes, sink);
      case initial_byte::type::special:
        return parse(tag<initial_byte::type::special>, byte, bytes, sink);
      default:
        throw cxx::cbor::unsupported("decoding given type is not yet supported");
    }
  }
} // namespace

auto ::cxx::cbor::decode(json::byte_stream const& stream) -> json
{
  byte_view data(stream.data(), std::size(stream));
  return decode(data);
}

auto ::cxx::cbor::decode(byte_view& bytes) -> json
{
  cxx::json json;
  bytes = parse(bytes, emplace_to(json));
  return json;
}
