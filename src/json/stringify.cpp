#include "inc/cxx/json.hpp"
#include <iomanip>
#include <sstream>

namespace
{
  auto const serializer =
      cxx::overload([](cxx::json::null_t) -> std::string { return "null"; },
                    [](std::int64_t x) -> std::string { return std::to_string(x); },
                    [](double x) -> std::string { return std::to_string(x); },
                    [](bool x) -> std::string { return x ? "true" : "false"; },
                    [](std::string const& x) -> std::string {
                      std::stringstream ss;
                      ss << std::quoted(x);
                      return ss.str();
                    },
                    [](cxx::json::array const& array) -> std::string {
                      if (std::empty(array)) return "[]";
                      std::stringstream ss;
                      ss << "[";
                      for (auto const& x : array) ss << cxx::to_string(x) << ", ";
                      auto ret = ss.str();
                      ret.pop_back();
                      ret.back() = ']';
                      return ret;
                    },
                    [](cxx::json::dictionary const& dict) -> std::string {
                      if (std::empty(dict)) return "{}";
                      std::stringstream ss;
                      ss << "{";
                      for (auto const& [key, value] : dict)
                      { ss << std::quoted(key) << ": " << cxx::to_string(value) << ", "; }
                      auto ret = ss.str();
                      ret.pop_back();
                      ret.back() = '}';
                      return ret;
                    },
                    [](cxx::json::byte_stream const&) -> std::string {
                      throw std::invalid_argument("bytes are not JSON serializable");
                    });
}

auto ::cxx::to_string(json const& object) -> std::string
{
  return cxx::visit(serializer, object);
}
