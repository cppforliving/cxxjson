#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"
#include "src/codec.hpp"
#include <algorithm>
#include <list>
#include <arpa/inet.h>

namespace
{
  template <typename T, typename = void>
  struct has_reserve : std::false_type {
  };

  template <typename T>
  struct has_reserve<
      T,
      std::void_t<decltype(std::declval<T&>().reserve(std::declval<typename T::size_type>()))>>
      : std::true_type {
  };

  template <typename T>
  constexpr bool has_reserve_v = has_reserve<T>::value;

  using initial_byte = cxx::detail::cbor::initial_byte;
  template <::cxx::codec::base_type<cxx::byte> t>
  using tag_t = std::integral_constant<::cxx::codec::base_type<cxx::byte>, t>;
  template <::cxx::codec::base_type<cxx::byte> t>
  constexpr tag_t<t> tag{};

  auto const read_half_float = [](cxx::json::byte_view& bytes, auto sink) {
    if (std::size(bytes) < sizeof(std::uint16_t))
      throw cxx::cbor::truncation_error("not enough data to decode floating point value");
    std::uint16_t x = 0;
    ::cxx::codec::read_from(x, bytes);
    x = ::cxx::codec::ntoh(x);
    auto const halfbits_to_floatbits = [](std::uint16_t h) -> std::uint32_t {
      std::uint16_t h_exp, h_sig;
      std::uint32_t f_sgn, f_exp, f_sig;

      h_exp = (h & 0x7c00u);
      f_sgn = ((std::uint32_t)h & 0x8000u) << 16;
      switch (h_exp)
      {
        case 0x0000u: /* 0 or subnormal */
          h_sig = (h & 0x03ffu);
          /* Signed zero */
          if (h_sig == 0) { return f_sgn; }
          /* Subnormal */
          h_sig = static_cast<std::uint16_t>(h_sig << 1);
          while ((h_sig & 0x0400u) == 0)
          {
            h_sig = static_cast<std::uint16_t>(h_sig << 1);
            h_exp++;
          }
          f_exp = ((std::uint32_t)(127 - 15 - h_exp)) << 23;
          f_sig = ((std::uint32_t)(h_sig & 0x03ffu)) << 13;
          return f_sgn + f_exp + f_sig;
        case 0x7c00u: /* inf or NaN */
          /* All-ones exponent and a copy of the significand */
          return f_sgn + 0x7f800000u + (((std::uint32_t)(h & 0x03ffu)) << 13);
        default: /* normalized */
          /* Just need to adjust the exponent and shift */
          return f_sgn + (((std::uint32_t)(h & 0x7fffu) + 0x1c000u) << 13);
      }
    };
    static_assert(alignof(std::uint32_t) == alignof(float));
    static_assert(sizeof(std::uint32_t) == sizeof(float));
    union horrible_cast
    {
      horrible_cast() = default;
      std::uint32_t uint = 0;
      float floating;
    } u;
    u.uint = halfbits_to_floatbits(x);
    double d = u.floating;
    sink(d);
    bytes.remove_prefix(sizeof(std::uint16_t));
  };

  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view, Sink, std::size_t = cxx::cbor::max_nesting);

  template <std::size_t S, typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view bytes, Sink sink)
  {
    if (std::size(bytes) < S) throw cxx::cbor::truncation_error("not enough data to decode json");
    auto n = ::cxx::codec::nbtoh<S>(bytes);
    if (n > std::numeric_limits<std::int64_t>::max())
      throw cxx::cbor::unsupported("integer value bigger than std::int64_t max");
    sink(static_cast<std::int64_t>(n));
    bytes.remove_prefix(S);
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::positive>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    auto const additional = cxx::detail::cbor::initial(byte)->additional;
    if (additional <= initial_byte::value::max_insitu)
    {
      sink(static_cast<std::int64_t>(additional));
      return bytes;
    }
    switch (additional)
    {
      case initial_byte::value::one_byte:
        return parse<sizeof(std::uint8_t)>(bytes, sink);
      case initial_byte::value::two_bytes:
        return parse<sizeof(std::uint16_t)>(bytes, sink);
      case initial_byte::value::four_bytes:
        return parse<sizeof(std::uint32_t)>(bytes, sink);
      case initial_byte::value::eigth_bytes:
        return parse<sizeof(std::uint64_t)>(bytes, sink);
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
  cxx::json::byte_view chunk(cxx::byte byte, cxx::json::byte_view bytes, Sink sink)
  {
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
  cxx::json::byte_view parse(tag_t<initial_byte::type::bytes>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    if (cxx::detail::cbor::initial(byte)->additional == initial_byte::value::indefinite)
    {
      auto const sentinel = [](auto const& data) {
        if (std::empty(data))
          throw cxx::cbor::truncation_error("break byte missing for indefinite-length collection");
        return data.front() != std::byte(0xff);
      };
      std::size_t length = 0;
      std::list<cxx::json::byte_view> chunks;
      auto collector = [&length, &chunks](cxx::json::byte_view view) mutable {
        length += std::size(view);
        chunks.push_back(view);
      };
      while (sentinel(bytes))
      {
        byte = bytes.front();
        bytes.remove_prefix(1);
        bytes = chunk(byte, bytes, collector);
      }
      bytes.remove_prefix(1);
      sink(std::move(chunks), length);
      return bytes;
    }
    else
      return chunk(byte, bytes, sink);
  }

  template <typename Stream>
  auto const merge_to = [](std::list<cxx::json::byte_view> chunks, std::size_t length, auto sink) {
    Stream stream;
    stream.reserve(length);
    for (auto const& item : chunks)
    {
      auto first = reinterpret_cast<typename Stream::const_pointer>(item.data());
      stream.insert(std::end(stream), first, first + std::size(item));
    }
    sink(std::move(stream));
  };

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::unicode>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink)
  {
    auto const adapter = cxx::overload(
        [&sink](cxx::json::byte_view x) {
          sink(std::string_view(reinterpret_cast<std::string_view::const_pointer>(x.data()),
                                std::size(x)));
        },
        [&sink](std::list<cxx::json::byte_view> chunks, std::size_t length) {
          merge_to<std::string>(std::move(chunks), length, sink);
        });
    return parse(tag<initial_byte::type::bytes>, byte, bytes, adapter);
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
        [impl](std::list<cxx::json::byte_view> chunks, std::size_t length) {
          merge_to<cxx::json::byte_stream>(chunks, length, impl);
        },
        impl);
  };

  template <typename Collection, typename Sink, typename Collector>
  cxx::json::byte_view collect(cxx::byte byte,
                               cxx::json::byte_view bytes,
                               Sink sink,
                               std::size_t level,
                               Collector collector)
  {
    if (--level == 0) throw cxx::cbor::unsupported("nesting level exceeds implementation limit");
    Collection col;
    if (cxx::detail::cbor::initial(byte)->additional == initial_byte::value::indefinite)
    {
      auto const sentinel = [](cxx::json::byte_view data) {
        if (std::empty(data))
          throw cxx::cbor::truncation_error("break byte missing for indefinite-length collection");
        return data.front() != std::byte(0xff);
      };
      while (sentinel(bytes))
      {
        if (std::size(col) == cxx::cbor::max_size)
          throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
        collector(bytes, col, level);
      }
      bytes.remove_prefix(1);
    }
    else
    {
      typename Collection::size_type size = 0;
      bytes = parse(tag<initial_byte::type::positive>, byte, bytes, [&size](std::int64_t x) {
        size = static_cast<typename Collection::size_type>(x);
      });
      if (size > cxx::cbor::max_size)
        throw cxx::cbor::unsupported("number of elements exceeds implementation limit");
      if constexpr (has_reserve_v<Collection>) col.reserve(size);
      while (size--) collector(bytes, col, level);
    }
    sink(std::move(col));
    return bytes;
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::array>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink,
                             std::size_t level)
  {
    auto const collector = [](auto& data, auto& collection, auto nesting) {
      data = parse(data, emplace_to(collection), nesting);
    };
    return collect<cxx::json::array>(byte, bytes, sink, level, collector);
  }

  template <typename Sink>
  cxx::json::byte_view parse(tag_t<initial_byte::type::dictionary>,
                             cxx::byte byte,
                             cxx::json::byte_view bytes,
                             Sink sink,
                             std::size_t level)
  {
    auto const collector = [](auto& data, auto& collection, auto nesting) {
      if (std::size(data) < 2)
        throw cxx::cbor::truncation_error("not enough data to decode dictionary key");
      auto const init = data.front();
      data.remove_prefix(1);
      if (cxx::detail::cbor::initial(init)->major != initial_byte::type::unicode)
        throw cxx::cbor::unsupported(
            "dictionary keys of type different thant unicode are not supported");
      std::string_view key;
      data = parse(tag<initial_byte::type::unicode>, init, data,
                   [&key](std::string_view x) { key = x; });
      data = parse(data, emplace_to(collection, key), nesting);
    };
    return collect<cxx::json::dictionary>(byte, bytes, sink, level, collector);
  }

  template <typename T>
  auto const floating_point_value = [](cxx::json::byte_view& bytes, auto sink) {
    if (std::size(bytes) < sizeof(T))
      throw cxx::cbor::truncation_error("not enough data to decode floating point value");
    T x = 0.0;
    ::cxx::codec::read_from(x, bytes);
    double ret = ::cxx::codec::ntoh(x);
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
      auto const x = static_cast<::cxx::codec::base_type<cxx::byte>>(v);
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
    auto const value = static_cast<::cxx::codec::base_type<cxx::byte>>(byte);
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
      case initial_byte::value::ieee_754_half:
        read_half_float(bytes, sink);
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
  cxx::json::byte_view parse(cxx::json::byte_view bytes, Sink sink, std::size_t level)
  {
    if (bytes.empty()) throw cxx::cbor::truncation_error("not enough data to decode json");
    auto const byte = bytes.front();
    bytes.remove_prefix(1);
    switch (cxx::detail::cbor::initial(byte)->major)
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
        return parse(tag<initial_byte::type::array>, byte, bytes, sink, level);
      case initial_byte::type::dictionary:
        return parse(tag<initial_byte::type::dictionary>, byte, bytes, sink, level);
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
