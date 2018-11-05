#pragma once
#include "test/catch.hpp"
#include "inc/cxx/json.hpp"

namespace Catch
{
  template <>
  struct StringMaker<cxx::json::byte_stream> {
    static std::string convert(::cxx::json::byte_stream const&);
  };
} // namespace Catch
