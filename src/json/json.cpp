#include "inc/cxx/json.hpp"

::cxx::json::json(std::initializer_list<cxx::array::value_type> init)
    : storage(cxx::array(std::move(init)))
{
}

::cxx::json::json(std::initializer_list<std::pair<key const, json>> init) : json()
{
  auto& doc = cxx::get<cxx::document>(*this);
  for (auto & [ k, v ] : init) {
    doc.emplace(std::piecewise_construct, std::forward_as_tuple(k.ptr, k.size),
                std::forward_as_tuple(std::move(v)));
  }
}

auto ::cxx::json::operator=(std::initializer_list<cxx::array::value_type> init) -> json&
{
  return (*this = cxx::json(std::move(init)));
}

auto ::cxx::json::operator=(std::initializer_list<std::pair<key const, json>> init) -> json&
{
  return (*this = cxx::json(std::move(init)));
}

auto ::cxx::json::operator[](std::string const& k) -> json&
{
  return cxx::get<cxx::document>(*this)[k];
}

auto ::cxx::json::operator[](std::string const& k) const -> json const&
{
  return cxx::get<cxx::document>(*this).at(k);
}

auto ::cxx::json::operator[](std::size_t k) -> json&
{
  return cxx::get<cxx::array>(*this).at(k);
}

auto ::cxx::json::operator[](std::size_t k) const -> json const&
{
  return cxx::get<cxx::array>(*this).at(k);
}

auto ::cxx::json::size() const noexcept -> std::size_t
{
  auto const func =
      cxx::overload([](cxx::null_t) -> std::size_t { return 0; },
                    [](auto const& x) -> std::size_t {
                      if
                        constexpr(traits::has_size_v<decltype(x)>) return std::size(x);
                      else
                        return 1;
                    });
  return cxx::visit(func, *this);
}

bool ::cxx::json::empty() const noexcept
{
  return size() == 0;
}
bool ::cxx::operator==(json const& lhs, json const& rhs) noexcept
{
  return to_object(lhs) == to_object(rhs);
}

bool ::cxx::operator!=(json const& lhs, json const& rhs) noexcept
{
  return !(lhs == rhs);
}
