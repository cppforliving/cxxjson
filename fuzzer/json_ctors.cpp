#include "inc/cxx/json.hpp"
#include "inc/cxx/cbor.hpp"
#include <string_view>
#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  cxx::json const json = std::string_view(reinterpret_cast<char const*>(Data), Size);
  auto const bytes = cxx::cbor::encode(json);
  return 0;
}
