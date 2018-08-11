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
    struct consts {
      using type = std::uint8_t;
      constexpr static type const null = 0xc0;
      constexpr static type const False = 0xc2;
      constexpr static type const True = 0xc3;
      constexpr static type const positive = 0xcc;
      constexpr static type const negative = 0xd0;
      constexpr static type const negative_initial = 0xe0;
      constexpr static std::int64_t const max_initial = 0x7f;
      constexpr static std::int64_t const min_initial = -0x20;
    };
    cxx::byte& assign(std::int64_t const x, cxx::json::byte_stream& stream)
    {
      auto const code =
          ::cxx::generic_codec::code(static_cast<std::uint64_t>((x < 0) ? (~x + 1) : x));
      auto const space = 1u << code;
      ::cxx::generic_codec::assure(stream, sizeof(cxx::byte) + space);
      stream.emplace_back(cxx::byte(code + ((x < 0) ? consts::negative : consts::positive)));

      auto const write = [&x, &stream](auto t) -> decltype(auto) {
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
          return write(std::uint8_t{});
        case sizeof(std::uint16_t):
          return write(std::uint16_t{});
        case sizeof(std::uint32_t):
          return write(std::uint32_t{});
        default:
          return write(std::uint64_t{});
      }
    }

    template <typename T>
    void encode(T const&, cxx::json::byte_stream&)
    {
      throw cxx::msgpack::unsupported("encoding is not yet supported");
    }

    cxx::byte& encode(std::int64_t x, cxx::json::byte_stream& stream)
    {
      auto const initial_byte = [&x, &stream] {
        if (x < consts::min_initial || x > consts::max_initial) return false;
        stream.emplace_back(
            cxx::byte((x < 0) ? ((0xff & x) | consts::negative_initial) : (0xff & x)));
        return true;
      };
      if (initial_byte()) return stream.back();
      return assign(x, stream);
    }

    void encode(cxx::json::null_t, cxx::json::byte_stream& stream)
    {
      stream.push_back(cxx::byte(consts::null));
    }

    void encode(bool b, cxx::json::byte_stream& stream)
    {
      stream.push_back(cxx::byte(b ? consts::True : consts::False));
    }

    void encode(std::string const& x, cxx::json::byte_stream& stream)
    {
      ::cxx::generic_codec::assure(stream, std::size(x) + sizeof(std::uint64_t) + 1);
      auto& init = assign(static_cast<std::int64_t>(std::size(x)), stream);
      init = cxx::byte(static_cast<std::uint8_t>(init) + 0xd);
      auto const first = reinterpret_cast<cxx::byte const*>(x.data());
      stream.insert(std::end(stream), first, first + std::size(x));
    }

    void encode(cxx::json::byte_stream const& x, cxx::json::byte_stream& stream)
    {
      ::cxx::generic_codec::assure(stream, std::size(x) + sizeof(std::uint64_t) + 1);
      auto& init = assign(static_cast<std::int64_t>(std::size(x)), stream);
      init = cxx::byte(static_cast<std::uint8_t>(init) - 0x8);
      auto const first = x.data();
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
