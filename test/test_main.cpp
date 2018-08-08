#define CATCH_CONFIG_MAIN
#include "test/catch_wrap.hpp"

auto ::Catch::StringMaker<::cxx::json::byte_stream>::convert(::cxx::json::byte_stream const& bytes)
    -> std::string
{
  std::string ret;
  ret.reserve(2 * std::size(bytes) + 3);
  ret.push_back('h');
  ret.push_back('\'');
  for (auto b : bytes)
  {
    auto h = static_cast<char>(static_cast<std::uint8_t>(b) >> 4);
    auto l = static_cast<char>(0xf & static_cast<std::uint8_t>(b));
    ret.push_back(h + (h > 9 ? ('a' - 0xa) : '0'));
    ret.push_back(l + (l > 9 ? ('a' - 0xa) : '0'));
  }
  ret.push_back('\'');
  return ret;
}
