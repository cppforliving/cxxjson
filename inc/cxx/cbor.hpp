#pragma once
#include <cxx/json.hpp>
#include <string_view>

namespace cxx
{
  struct cbor {
    struct error : virtual std::runtime_error {
      using std::runtime_error::runtime_error;
      virtual ~error() = default;
    };
    struct buffer_error : virtual error {
      using error::error;
      virtual ~buffer_error() = default;
    };
    struct unsupported : virtual error {
      using error::error;
      virtual ~unsupported() = default;
    };
    struct data_error : virtual error {
      using error::error;
      virtual ~data_error() = default;
    };
    using byte_view = std::basic_string_view<json::byte_stream::value_type>;
    static json::byte_stream encode(json const&) noexcept;
    static json decode(json::byte_stream const&);
    static json decode(byte_view&);
  };
}
