#pragma once
#include <cxx/json.hpp>
#include <vector>
#include <cstddef>

namespace cxx
{
  using byte = std::uint8_t;
  struct cbor {
    using byte_stream = std::vector<::cxx::byte>;
    static byte_stream encode(json const&) noexcept;
    static json decode(byte_stream const&);
  };
}
