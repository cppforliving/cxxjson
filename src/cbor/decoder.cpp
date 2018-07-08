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

  constexpr auto const ntohb = cxx::overload(
      [](std::uint8_t, cxx::json::byte_view bytes) -> std::int64_t {
        return static_cast<std::int64_t>(bytes.front());
      },
      [](std::uint16_t x, cxx::json::byte_view bytes) -> std::int64_t {
        cxx::read_to(x, bytes);
        return cxx::ntoh(x);
      },
      [](std::uint32_t x, cxx::json::byte_view bytes) -> std::int64_t {
        cxx::read_to(x, bytes);
        return cxx::ntoh(x);
      },
      [](std::uint64_t x, cxx::json::byte_view bytes) -> std::int64_t {
        cxx::read_to(x, bytes);
        x = cxx::ntoh(x);
        if (x > std::numeric_limits<std::int64_t>::max())
          throw cxx::cbor::unsupported("integer value bigger than std::int64_t max");
        return static_cast<std::int64_t>(x);
      });

  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view, Sink);

  template <typename Int, typename Sink>
  cxx::json::byte_view parse(Int, cxx::json::byte_view bytes, Sink sink)
  {
    if (std::size(bytes) < sizeof(Int))
      throw cxx::cbor::truncation_error("not enough data to decode json");
    sink(ntohb(Int{}, bytes));
    bytes.remove_prefix(sizeof(Int));
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::positive>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    auto const additional = cxx::codec::cbor::initial(byte)->additional;
    if (additional <= initial_byte::value::max_insitu)
    {
      sink(static_cast<std::int64_t>(additional));
      return bytes;
    }
    switch (additional)
    {
      case initial_byte::value::one_byte:
        return parse(std::uint8_t{0}, bytes, sink);
      case initial_byte::value::two_bytes:
        return parse(std::uint16_t{0}, bytes, sink);
      case initial_byte::value::four_bytes:
        return parse(std::uint32_t{0}, bytes, sink);
      case initial_byte::value::eigth_bytes:
        return parse(std::uint64_t{0}, bytes, sink);
      default:
        throw cxx::cbor::data_error("meaningless additional data in initial byte");
    }
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::negative>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    return parse(tag<initial_byte::type::positive>, byte, bytes,
                 [&sink](std::int64_t x) { sink(-x - 1); });
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::bytes>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    if (cxx::codec::cbor::initial(byte)->additional == initial_byte::value::infinite)
      throw cxx::cbor::unsupported("infinite size collections are not supported");
    std::size_t const size = [&] {
      std::size_t n = 0;
      auto const leftovers = parse(tag<initial_byte::type::positive>, byte, bytes,
                                   [&n](std::int64_t x) { n = static_cast<std::size_t>(x); });
      if (std::size(leftovers) < n)
        throw cxx::cbor::truncation_error("not enough data to decode byte_stream");
      bytes = leftovers;
      return n;
    }();
    sink(cxx::json::byte_view(bytes.data(), size));
    bytes.remove_prefix(size);
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::unicode>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    return parse(tag<initial_byte::type::bytes>, byte, bytes, [&sink](cxx::json::byte_view x) {
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
        [impl](cxx::json::byte_view bytes) {
          impl(cxx::json::byte_stream(bytes.data(), bytes.data() + std::size(bytes)));
        },
        impl);
  };

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::array>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    cxx::json::array array;
    if (cxx::codec::cbor::initial(byte)->additional == initial_byte::value::infinite)
    {
      auto const sentinel = [](cxx::json::byte_view data) {
        if (std::empty(data))
          throw cxx::cbor::truncation_error("break byte missing for indefinite-length array");
        return data.front() != std::byte(0xff);
      };
      while (sentinel(bytes))
      {
        if (std::size(array) == cxx::cbor::max_size)
          throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
        bytes = parse(bytes, emplace_to(array));
      }
      bytes.remove_prefix(1);
    }
    else
    {
      cxx::json::array::size_type size = 0;
      bytes = parse(tag<initial_byte::type::positive>, byte, bytes, [&size](std::int64_t x) {
        size = static_cast<cxx::json::array::size_type>(x);
      });
      if (size > cxx::cbor::max_size)
        throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
      array.reserve(size);
      while (size--) bytes = parse(bytes, emplace_to(array));
    }
    sink(std::move(array));
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::dictionary>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    if (cxx::codec::cbor::initial(byte)->additional == initial_byte::value::infinite)
      throw cxx::cbor::unsupported("infinite size collections are not supported");
    cxx::json::dictionary::size_type size = 0;
    bytes = parse(tag<initial_byte::type::positive>, byte, bytes, [&size](std::int64_t x) {
      size = static_cast<cxx::json::dictionary::size_type>(x);
    });
    if (size > cxx::cbor::max_size)
      throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
    cxx::json::dictionary dict;
    while (size--)
    {
      if (std::size(bytes) < 2)
        throw cxx::cbor::truncation_error("not enough data to decode dictionary key");
      auto const init = bytes.front();
      bytes.remove_prefix(1);
      if (cxx::codec::cbor::initial(bytes.front())->major != initial_byte::type::unicode)
        throw cxx::cbor::unsupported(
            "dictionary keys of type different thant unicode are not supported");
      std::string_view key;
      bytes = parse(tag<initial_byte::type::unicode>, init, bytes,
                    [&key](std::string_view x) { key = x; });
      bytes = parse(bytes, emplace_to(dict, key));
    }
    sink(std::move(dict));
    return bytes;
  }

  template <typename T>
  auto const floating_point_value = [](cxx::json::byte_view& bytes, auto sink) {
    if (std::size(bytes) < sizeof(T))
      throw cxx::cbor::truncation_error("not enough data to decode floating point value");
    T x = 0.0;
    cxx::read_to(x, bytes);
    double ret = cxx::ntoh(x);
    sink(ret);
    bytes.remove_prefix(sizeof(T));
  };

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::special>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
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
      case initial_byte::value::Simple:
        if (std::empty(bytes))
          throw cxx::cbor::truncation_error("not enough data to decode simple value");
        simple_value(bytes.at(0));
        bytes.remove_prefix(1);
        break;
      case initial_byte::value::ieee_754_single:
        floating_point_value<float>(bytes, sink);
        break;
      case initial_byte::value::ieee_754_double:
        floating_point_value<double>(bytes, sink);
        break;
      default:
        throw cxx::cbor::unsupported("decoding given type is not yet supported");
    }
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view bytes, Sink sink)
  {
    if (bytes.empty()) throw cxx::cbor::truncation_error("not enough data to decode json");
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
  cxx::json::byte_view data(stream.data(), std::size(stream));
  return decode(data);
}

auto ::cxx::cbor::decode(json::byte_view& bytes) -> json
{
  cxx::json json;
  bytes = parse(bytes, emplace_to(json));
  return json;
}
