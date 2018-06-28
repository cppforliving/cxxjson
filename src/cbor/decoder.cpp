#include "inc/cxx/cbor.hpp"
#include "src/cbor/initial_byte.hpp"

namespace
{
  using initial_byte = cxx::codec::cbor::initial_byte;
  template <cxx::codec::cbor::base_type<cxx::byte> t>
  using tag_t = std::integral_constant<cxx::codec::cbor::base_type<cxx::byte>, t>;
  template <cxx::codec::cbor::base_type<cxx::byte> t>
  constexpr tag_t<t> tag{};

  std::pair<std::int64_t, cxx::cbor::byte_view> parse(tag_t<initial_byte::type::positive>,
                                                      cxx::byte byte,
                                                      cxx::cbor::byte_view bytes)
  {
    auto const additional = cxx::codec::cbor::initial(byte)->additional;
    if (additional <= initial_byte::value::max_insitu) return {std::int64_t(additional), bytes};
    if (additional > initial_byte::value::eigth_bytes)
      throw cxx::cbor::data_error("meaningless additional data in initial byte");
    auto const space = (1u << (0x3 & (additional - initial_byte::value::max_insitu - 1)));
    if (std::size(bytes) < space) throw cxx::cbor::buffer_error("not enough data to decode json");
    if (space == 1) {
      auto const x = bytes.front();
      bytes.remove_prefix(1);
      return {std::int64_t(x), bytes};
    }
    throw cxx::cbor::unsupported("integer format not yet supported");
  }
}

auto ::cxx::cbor::decode(json::byte_stream const& stream) -> std::pair<json, byte_view>
{
  return decode(byte_view(stream.data(), std::size(stream)));
}

auto ::cxx::cbor::decode(byte_view bytes) -> std::pair<json, byte_view>
{
  if (bytes.empty()) throw cxx::cbor::buffer_error("not enough data to decode json");
  auto const byte = bytes.front();
  bytes.remove_prefix(1);
  switch (codec::cbor::initial(byte)->major)
  {
    case initial_byte::type::positive:
      return parse(tag<initial_byte::type::positive>, byte, bytes);
    default:
      throw cxx::cbor::unsupported("decoding given type is not yet supported");
  }
}
