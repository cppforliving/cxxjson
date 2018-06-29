#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"
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
        x = (static_cast<std::uint64_t>(
                 ntohl(*reinterpret_cast<std::uint32_t const*>(bytes.data())))
             << 32) |
            ntohl(*reinterpret_cast<std::uint32_t const*>(bytes.data() + sizeof(std::uint32_t)));
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
      default:
        throw cxx::cbor::unsupported("decoding given type is not yet supported");
    }
  }
}

auto ::cxx::cbor::decode(json::byte_stream const& stream) -> json
{
  byte_view data(stream.data(), std::size(stream));
  return decode(data);
}

auto ::cxx::cbor::decode(byte_view& bytes) -> json
{
  cxx::json json;
  auto sink = cxx::overload([&json](std::int64_t x) { json = x; }, [](auto const&) {});
  bytes = parse(bytes, sink);
  return json;
}
