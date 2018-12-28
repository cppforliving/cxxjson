#include "inc/cxx/msgpack.hpp"
#include "src/codec.hpp"

namespace
{
  namespace detail
  {
    struct consts {
      using type = ::cxx::codec::base_type<::cxx::byte>;
      constexpr static type const null = 0xc0;
      constexpr static type const False = 0xc2;
      constexpr static type const True = 0xc3;
      constexpr static type const positive = 0xcc;
      constexpr static type const negative = 0xd0;
      constexpr static type const negative_initial = 0xe0;
      constexpr static type const string = 0xdb;
      constexpr static type const bin = 0xc6;
      constexpr static type const array = 0xdd;
      constexpr static type const dictionary = 0xdf;
      constexpr static type const floating = 0xcb;
      constexpr static std::int64_t const max_initial = 0x7f;
      constexpr static std::int64_t const min_initial = -0x20;
    };

    auto const insert = [](auto x,
                           cxx::output_parameter<cxx::json::byte_stream> stream) -> decltype(auto) {
      using T = std::decay_t<decltype(x)>;
      auto it = stream->insert(stream->end(), sizeof(T), {});
      ::cxx::codec::write_to(::cxx::codec::hton(static_cast<T>(x)), std::addressof(*it));
      return *(--it);
    };

    auto const collect = [](auto const& x,
                            cxx::output_parameter<cxx::json::byte_stream> stream,
                            auto code,
                            auto sink) {
      auto const size = std::size(x);
      using value_type = typename std::decay_t<decltype(x)>::value_type;
      ::cxx::codec::assure(cxx::by_ref(stream),
                           size * sizeof(value_type) + sizeof(std::uint32_t) + 1);

      stream->emplace_back(cxx::byte(code));
      insert(static_cast<std::uint32_t>(size), cxx::by_ref(stream));
      sink(x, cxx::by_ref(stream));
    };

    void encode(cxx::json const&, cxx::output_parameter<cxx::json::byte_stream>);

    cxx::byte& assign(std::int64_t const x, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const n = static_cast<std::uint64_t>(x);
      auto const code = ::cxx::codec::code((x < 0) ? (~n + 1) : n);
      auto const space = 1u << code;
      ::cxx::codec::assure(cxx::by_ref(stream), sizeof(cxx::byte) + space);
      stream->emplace_back(cxx::byte(code + ((x < 0) ? consts::negative : consts::positive)));

      /// space optimization - adjust number of bytes needed to store a value
      /// is it really needed? use 8-bytes allways instead
      switch (space)
      {
        case sizeof(std::uint8_t):
          return insert(static_cast<std::uint8_t>(x), cxx::by_ref(stream));
        case sizeof(std::uint16_t):
          return insert(static_cast<std::uint16_t>(x), cxx::by_ref(stream));
        case sizeof(std::uint32_t):
          return insert(static_cast<std::uint32_t>(x), cxx::by_ref(stream));
        default:
          return insert(static_cast<std::uint64_t>(x), cxx::by_ref(stream));
      }
    }

    cxx::byte& encode(std::int64_t x, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const initial_byte = [&x, &stream] {
        if (x < consts::min_initial || x > consts::max_initial) return false;
        stream->emplace_back(
            cxx::byte((x < 0) ? ((0xff & x) | consts::negative_initial) : (0xff & x)));
        return true;
      };
      if (initial_byte()) return stream->back();
      return assign(x, cxx::by_ref(stream));
    }

    void encode(cxx::json::null_t, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      stream->push_back(cxx::byte(consts::null));
    }

    void encode(bool b, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      stream->push_back(cxx::byte(b ? consts::True : consts::False));
    }

    void encode(std::string const& x, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const sink = [](auto const& y, cxx::output_parameter<cxx::json::byte_stream> out) {
        auto const first = reinterpret_cast<cxx::byte const*>(y.data());
        out->insert(out->end(), first, first + std::size(y));
      };
      collect(x, cxx::by_ref(stream), consts::string, sink);
    }

    void encode(cxx::json::byte_stream const& x,
                cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const sink = [](auto const& y, cxx::output_parameter<cxx::json::byte_stream> out) {
        auto const first = y.data();
        out->insert(out->end(), first, first + std::size(y));
      };
      collect(x, cxx::by_ref(stream), consts::bin, sink);
    }

    void encode(cxx::json::array const& x, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const sink = [](auto const& y, cxx::output_parameter<cxx::json::byte_stream> out) {
        for (auto const& value : y) encode(value, cxx::by_ref(out));
      };
      collect(x, cxx::by_ref(stream), consts::array, sink);
    }

    void encode(cxx::json::dictionary const& x,
                cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      auto const sink = [](auto const& y, cxx::output_parameter<cxx::json::byte_stream> out) {
        for (auto const& [key, value] : y)
        {
          encode(key, cxx::by_ref(out));
          encode(value, cxx::by_ref(out));
        }
      };
      collect(x, cxx::by_ref(stream), consts::dictionary, sink);
    }

    void encode(double x, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      ::cxx::codec::assure(cxx::by_ref(stream), sizeof(double) + 1);
      x = ::cxx::codec::hton(x);
      auto const* first = static_cast<cxx::byte const*>(static_cast<void const*>(&x));
      stream->emplace_back(cxx::byte(consts::floating));
      stream->insert(stream->end(), first, first + sizeof(double));
    }

    void encode(cxx::json const& json, cxx::output_parameter<cxx::json::byte_stream> stream)
    {
      cxx::visit([&stream](auto const& x) { detail::encode(x, cxx::by_ref(stream)); }, json);
    }
  } // namespace detail
} // namespace

auto ::cxx::msgpack::encode(json const& obj) -> json::byte_stream
{
  auto stream = ::cxx::codec::reserved<json::byte_stream>(sizeof(json));
  detail::encode(obj, cxx::by_ref(stream));
  return stream;
}
