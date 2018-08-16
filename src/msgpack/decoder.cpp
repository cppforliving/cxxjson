#include "inc/cxx/msgpack.hpp"
#include "src/codec.hpp"

namespace
{
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

  template <typename T, typename Sink>
  cxx::json::byte_view parse(cxx::codec::numbyte const init,
                             cxx::json::byte_view const bytes,
                             Sink sink,
                             std::size_t const /*level*/)
  {
    if constexpr (std::is_same_v<T, std::int64_t>)
    {
      std::size_t const space = 1 << (init - 0xcc);
      if (std::size(bytes) < space)
        throw cxx::msgpack::truncation_error("not enough data to decode json");
      std::uint64_t x = 0;
      switch (space)
      {
        case sizeof(std::uint8_t):
          x = ::cxx::codec::nbtoh<sizeof(std::uint8_t)>(bytes);
          break;
        case sizeof(std::uint16_t):
          x = ::cxx::codec::nbtoh<sizeof(std::uint16_t)>(bytes);
          break;
        case sizeof(std::uint32_t):
          x = ::cxx::codec::nbtoh<sizeof(std::uint32_t)>(bytes);
          break;
        case sizeof(std::uint64_t):
          x = ::cxx::codec::nbtoh<sizeof(std::uint64_t)>(bytes);
          break;
        default:
          throw ::cxx::msgpack::data_error("invalid integer code");
      }
      if (x > std::numeric_limits<std::int64_t>::max())
        throw cxx::msgpack::unsupported("integer value bigger than std::int64_t max");
      sink(static_cast<std::int64_t>(x));
      return bytes.substr(space);
    }
    else
    {
      throw cxx::msgpack::unsupported("decoding given type is not yet supported");
    }
  }

  template <typename Sink>
  cxx::json::byte_view parse(cxx::json::byte_view const bytes,
                             Sink sink,
                             std::size_t const level = ::cxx::codec::max_nesting)
  {
    auto const init = static_cast<cxx::codec::numbyte>(bytes.front());
    if (init < 0x80)
    {
      sink(static_cast<std::int64_t>(init));
      return bytes;
    }
    switch (init)
    {
      case 0xcc:
      case 0xcd:
      case 0xce:
      case 0xcf:
        return parse<std::int64_t>(init, bytes.substr(1), sink, level);
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
