#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace cxx::meta
{
  template <typename T>
  using quote = std::common_type<T>;

  template <bool... B>
  constexpr std::bool_constant<(B || ...)> any{};

  template <typename... T>
  struct type_list {
    template <typename U>
    static constexpr auto contains = any<std::is_same_v<U, T>...>;

    template <template <typename...> typename F>
    using apply = typename quote<F<T...>>::type;

    template <template <typename...> typename F, typename Head, typename... Tail>
    static constexpr auto find_impl() noexcept
    {
      if
        constexpr(F<Head>::value) return quote<Head>{};
      else if
        constexpr(sizeof...(Tail) > 0) return find_impl<F, Tail...>();
      else
        throw 1; // throw in constexpr function is a compile time error
    }

    template <template <typename...> typename F>
    using find = typename decltype(find_impl<F, T...>())::type;
  };
}

namespace cxx
{
  template <typename... F>
  struct overload : F... {
    explicit constexpr overload(F... f) noexcept : F(std::move(f))... {}
    using F::operator()...;
  };

  template <typename... F>
  overload(F...)->overload<F...>;

  template <typename T>
  struct is_convertible_to {
    template <typename... U>
    using any_of = std::bool_constant<(std::is_convertible_v<T, U> || ...) ||
                                      (std::is_constructible_v<U, T> || ...)>;

    template <typename U>
    using type = any_of<U>;
  };

  enum class null_t
  {
  };
  constexpr null_t null{};

  constexpr bool operator==(null_t, null_t) noexcept { return true; }
  constexpr bool operator!=(null_t, null_t) noexcept { return false; }

  struct json;

  using document = std::map<std::string, json>;

  using array = std::vector<json>;

  using alternatives =
      meta::type_list<document, std::int64_t, array, std::string, double, bool, null_t>;

  template <typename T>
  constexpr auto is_alternative = alternatives::contains<T>;

  template <typename T>
  using compatibile_alternative = alternatives::find<is_convertible_to<T>::template type>;

  template <typename T>
  constexpr bool is_compatibile = !std::is_same_v<json, T> && !is_alternative<T> &&
                                  alternatives::apply<is_convertible_to<T>::template any_of>::value;

  using object = alternatives::apply<std::variant>;

  struct json : object {
    struct error : virtual std::runtime_error {
      using std::runtime_error::runtime_error;
    };
    struct bad_access : virtual error {
      using error::error;
    };
    using object::object;
    using object::operator=;

    template <typename T, typename = std::enable_if_t<is_compatibile<std::decay_t<T>>>>
    explicit json(T&& t) noexcept(noexcept(compatibile_alternative<std::decay_t<T>>{t}))
        : json(compatibile_alternative<std::decay_t<T>>{t})
    {
    }

    template <typename T, typename = std::enable_if_t<is_compatibile<std::decay_t<T>>>>
    json& operator=(T const& t) noexcept(noexcept(compatibile_alternative<std::decay_t<T>>{t}))
    {
      this->object::emplace<compatibile_alternative<std::decay_t<T>>>(t);
      return *this;
    }

    json& operator[](std::string const&);
    json const& operator[](std::string const&) const;
  };

  auto const to_object = overload([](json& x) -> object& { return x; },
                                  [](json const& x) -> object const& { return x; });

  template <typename T>
  auto const get =
      overload([](json const& x) -> decltype(auto) { return std::get<T>(to_object(x)); },
               [](json& x) -> decltype(auto) { return std::get<T>(to_object(x)); });

  auto const visit =
      overload([](auto&& f, json& x) -> decltype(
                   auto) { return std::visit(std::forward<decltype(f)>(f), to_object(x)); },
               [](auto&& f, json const& x) -> decltype(auto) {
                 return std::visit(std::forward<decltype(f)>(f), to_object(x));
               });

  json& json::operator[](std::string const& key)
  {
    auto const func = overload(
        [](auto const&) -> json& {
          throw json::bad_access{"object does not support item access by string key"};
        },
        [&key](cxx::document& doc) -> json& { return doc.at(key); });
    return cxx::visit(func, *this);
  }

  template <typename T,
            typename = std::enable_if_t<is_alternative<std::decay_t<T>> ||
                                        is_compatibile<std::decay_t<T>>>>
  bool operator==(json const& j, T const& rhs) noexcept
  {
    using type = std::conditional_t<is_alternative<std::decay_t<T>>, T,
                                    compatibile_alternative<std::decay_t<T>>>;
    auto const func = [&rhs](auto const& lhs) -> bool {
      if
        constexpr(std::is_same_v<decltype(lhs), type const&>) return lhs == rhs;
      else
        return false;
    };
    return cxx::visit(func, j);
  }

  template <typename T,
            typename = std::enable_if_t<is_alternative<std::decay_t<T>> ||
                                        is_compatibile<std::decay_t<T>>>>
  bool operator==(T const& lhs, json const& rhs) noexcept
  {
    return rhs == lhs;
  }

  template <typename T,
            typename = std::enable_if_t<is_alternative<std::decay_t<T>> ||
                                        is_compatibile<std::decay_t<T>>>>
  bool operator!=(json const& lhs, T const& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template <typename T,
            typename = std::enable_if_t<is_alternative<std::decay_t<T>> ||
                                        is_compatibile<std::decay_t<T>>>>
  bool operator!=(T const& lhs, json const& rhs) noexcept
  {
    return !(lhs == rhs);
  }
}
