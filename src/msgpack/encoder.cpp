#include "inc/cxx/msgpack.hpp"
#include "src/codec.hpp"

namespace
{
  template <typename T>
  auto const reserved = [](typename T::size_type capa) -> T {
    T ret;
    ret.reserve(capa);
    return ret;
  };

  namespace detail
  {
    template <typename T>
    void encode(T const&, cxx::json::byte_stream&)
    {
      throw cxx::msgpack::unsupported("encoding is not yet supported");
    }

    cxx::byte& encode(std::int64_t x, cxx::json::byte_stream& stream)
    {
      /// initial byte positive number
      if (x >= 0 && x < 0x20) return stream.emplace_back(cxx::byte(static_cast<std::uint8_t>(x)));

      /// initial byte negative number
      if (x < 0 && x > -0x21) return stream.emplace_back(cxx::byte((0xff & x) | 0xe0));
      auto const code =
          ::cxx::generic_codec::code(static_cast<std::uint64_t>((x < 0) ? (~x + 1) : x));
      auto const space = 1u << code;
      ::cxx::generic_codec::assure(stream, sizeof(cxx::byte) + space);
      stream.emplace_back(cxx::byte(code + ((x < 0) ? 0xd0 : 0xcc)));

      auto const assign = [&x, &stream](auto t) -> decltype(auto) {
        using T = std::decay_t<decltype(t)>;
        auto it = stream.insert(std::end(stream), sizeof(T), {});
        ::cxx::generic_codec::write_to(::cxx::generic_codec::hton(static_cast<T>(x)),
                                       std::addressof(*it));
        return *(--it);
      };

      /// space optimization - adjust number of bytes needed to store a value
      /// is it really needed? use 8-bytes allways instead
      switch (space)
      {
        case sizeof(std::uint8_t):
          return assign(std::uint8_t{});
        case sizeof(std::uint16_t):
          return assign(std::uint16_t{});
        case sizeof(std::uint32_t):
          return assign(std::uint32_t{});
        default:
          return assign(std::uint64_t{});
      }
    }

    void encode(cxx::json::null_t, cxx::json::byte_stream& stream)
    {
      stream.push_back(cxx::byte(0xc0));
    }

    void encode(bool b, cxx::json::byte_stream& stream)
    {
      stream.push_back(cxx::byte(b ? 0xc3 : 0xc2));
    }

    void encode(std::string const& x, cxx::json::byte_stream& stream)
    {
      ::cxx::generic_codec::assure(stream, std::size(x) + sizeof(std::uint64_t) + 1);
      auto& init = encode(static_cast<std::int64_t>(std::size(x)), stream);
      if (init == cxx::byte(std::size(x))) { init |= cxx::byte(0xa0); }
      else
      {
        init = cxx::byte(static_cast<std::uint8_t>(init) + 0xd);
      }
      auto const first = reinterpret_cast<cxx::byte const*>(x.data());
      stream.insert(std::end(stream), first, first + std::size(x));
    }

    void encode(cxx::json const& json, cxx::json::byte_stream& stream) noexcept
    {
      cxx::visit([&stream](auto const& x) { detail::encode(x, stream); }, json);
    }
  } // namespace detail
} // namespace

auto ::cxx::msgpack::encode(json const& obj) noexcept -> json::byte_stream
{
  auto stream = reserved<json::byte_stream>(sizeof(json));
  detail::encode(obj, stream);
  return stream;
}
