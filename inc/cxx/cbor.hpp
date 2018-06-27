#pragma once
#include <cxx/json.hpp>
#include <vector>

namespace cxx
{
  struct cbor {
    static json::byte_stream encode(json const&) noexcept;
    static json decode(json::byte_stream const&);
  };
}
