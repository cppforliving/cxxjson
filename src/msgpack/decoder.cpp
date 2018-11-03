#include "inc/cxx/msgpack.hpp"
#include "src/codec.hpp"

namespace
{
  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view const,
                             Sink,
                             std::size_t = ::cxx::codec::max_nesting);

  auto const emplace_to = [](auto& target, std::string_view key = std::string_view()) {
    using target_t = std::decay_t<decltype(target)>;
    using key_t = cxx::json::dictionary::key_type;
    return [&target, key](auto&& x) {
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
  };

  template <bool isSigned, typename T>
  using Int = std::conditional_t<isSigned, std::make_signed_t<T>, std::make_unsigned_t<T>>;

  template <bool isSigned>
  auto read_int64_t(std::size_t const space, cxx::json::byte_view const bytes)
      -> Int<isSigned, std::int64_t>
  {
    if (std::size(bytes) < space)
      throw cxx::msgpack::truncation_error("not enough data to decode json");
    switch (space)
    {
      case sizeof(std::int8_t):
        return static_cast<Int<isSigned, std::int8_t>>(
            ::cxx::codec::nbtoh<sizeof(std::int8_t)>(bytes));
      case sizeof(std::int16_t):
        return static_cast<Int<isSigned, std::int16_t>>(
            ::cxx::codec::nbtoh<sizeof(std::int16_t)>(bytes));
      case sizeof(std::int32_t):
        return static_cast<Int<isSigned, std::int32_t>>(
            ::cxx::codec::nbtoh<sizeof(std::int32_t)>(bytes));
      default:
        return static_cast<Int<isSigned, std::int64_t>>(
            ::cxx::codec::nbtoh<sizeof(std::int64_t)>(bytes));
    }
  }

  template <typename T, typename Sink>
  cxx::json::byte_view parse(cxx::codec::numbyte const init,
                             cxx::json::byte_view const bytes,
                             Sink sink,
                             std::size_t level)
  {
    (void)level;
    if constexpr (std::is_same_v<T, std::uint64_t>)
    {
      auto const space = 1u << (init - 0xcc);
      auto const x = read_int64_t<false>(space, bytes);
      if (x > std::numeric_limits<std::int64_t>::max())
        throw cxx::msgpack::unsupported("integer value bigger than std::int64_t max");
      sink(static_cast<std::int64_t>(x));
      return bytes.substr(space);
    }
    if constexpr (std::is_same_v<T, std::int64_t>)
    {
      auto const space = 1u << (init - 0xd0);
      sink(read_int64_t<true>(space, bytes));
      return bytes.substr(space);
    }
    if constexpr (std::is_same_v<T, std::string>)
    {
      auto leftovers = bytes;
      std::size_t const size = [init, &leftovers]() -> std::size_t {
        if ((init & 0xe0) == 0xa0) return init & 0x1f;
        std::size_t space = 1u << (init - 0xd9);
        auto const s = static_cast<std::size_t>(read_int64_t<false>(space, leftovers));
        leftovers.remove_prefix(space);
        return s;
      }();
      if (std::size(leftovers) < size)
        throw cxx::msgpack::truncation_error("not enough data to decode json");
      sink(std::string_view(reinterpret_cast<std::string_view::const_pointer>(leftovers.data()),
                            size));
      return leftovers.substr(size);
    }
    if constexpr (std::is_same_v<T, cxx::json::byte_stream>)
    {
      auto leftovers = bytes;
      std::size_t const space = 1u << (init - 0xc4);
      std::size_t const size = static_cast<std::size_t>(read_int64_t<false>(space, leftovers));
      leftovers.remove_prefix(space);

      if (std::size(leftovers) < size)
        throw cxx::msgpack::truncation_error("not enough data to decode json");
      cxx::json::byte_stream stream;
      stream.reserve(size);
      stream.insert(std::end(stream), std::begin(leftovers),
                    std::next(std::begin(leftovers),
                              static_cast<cxx::json::byte_view::difference_type>(size)));
      sink(std::move(stream));
      return leftovers.substr(size);
    }
    if constexpr (std::is_same_v<T, cxx::json::array>)
    {
      if (--level == 0)
        throw cxx::msgpack::unsupported("nesting level exceeds implementation limit");
      auto leftovers = bytes;
      std::size_t size = [init, &leftovers]() -> std::size_t {
        if ((init & 0xf0) == 0x90) return init & 0x0f;
        std::size_t space = 1u << (init - 0xdb);
        auto const s = static_cast<std::size_t>(read_int64_t<false>(space, leftovers));
        leftovers.remove_prefix(space);
        return s;
      }();
      if (std::size(leftovers) < size)
        throw cxx::msgpack::truncation_error("not enough data to decode json");
      cxx::json::array array;
      array.reserve(size);
      while (size--) { leftovers = parse(leftovers, emplace_to(array), level); }
      sink(std::move(array));
      return leftovers;
    }
    else
    {
      throw cxx::msgpack::unsupported("decoding given type is not yet supported");
    }
  }

  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view const bytes, Sink sink, std::size_t const level)
  {
    if (std::empty(bytes)) throw cxx::msgpack::truncation_error("not enough data to decode json");
    auto const init = static_cast<cxx::codec::numbyte>(bytes.front());
    auto leftovers = bytes.substr(sizeof(init));
    if (init < 0x80)
    {
      sink(static_cast<std::int64_t>(init));
      return leftovers;
    }
    if (init >= 0xe0)
    {
      sink(static_cast<std::int64_t>(static_cast<std::int8_t>(init)));
      return leftovers;
    }
    if ((init & 0xe0) == 0xa0) { return parse<std::string>(init, leftovers, sink, level); }
    if ((init & 0xf0) == 0x90) { return parse<cxx::json::array>(init, leftovers, sink, level); }
    switch (init)
    {
      case 0xc0:
        sink(cxx::json::null);
        return leftovers;
      case 0xc2:
        sink(false);
        return leftovers;
      case 0xc3:
        sink(true);
        return leftovers;
      case 0xc4:
      case 0xc5:
      case 0xc6:
        return parse<cxx::json::byte_stream>(init, leftovers, sink, level);
      case 0xcc:
      case 0xcd:
      case 0xce:
      case 0xcf:
        return parse<std::uint64_t>(init, leftovers, sink, level);
      case 0xd0:
      case 0xd1:
      case 0xd2:
      case 0xd3:
        return parse<std::int64_t>(init, leftovers, sink, level);
      case 0xd9:
      case 0xda:
      case 0xdb:
        return parse<std::string>(init, leftovers, sink, level);
      case 0xdc:
      case 0xdd:
        return parse<cxx::json::array>(init, leftovers, sink, level);
      default:
        throw cxx::msgpack::unsupported("decoding given type is not yet supported");
    }
  }
} // namespace

auto ::cxx::msgpack::decode(json::byte_stream const& stream) -> json
{
  cxx::json::byte_view data(stream.data(), std::size(stream));
  return decode(data);
}

auto ::cxx::msgpack::decode(json::byte_view& bytes) -> json
{
  cxx::json json;
  bytes = parse(bytes, emplace_to(json));
  return json;
}
