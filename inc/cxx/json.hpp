#pragma once

#include <variant>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace cxx::traits
{
  /*
   *
   */
  template <typename T, typename = void>
  struct has_size : std::false_type {
  };

  template <typename T>
  struct has_size<
      T,
      std::void_t<decltype(std::declval<std::size_t&>() = (std::declval<T const&>().size()))>>
      : std::true_type {
  };

  template <typename T>
  constexpr bool has_size_v = has_size<T>::value;

  /*
   *
   */
  template <typename T>
  struct is_convertible_to {
    template <typename... U>
    using any_of = std::bool_constant<(std::is_convertible_v<T, U> || ...) ||
                                      (std::is_constructible_v<U, T> || ...)>;

    template <typename U>
    using type = any_of<U>;
  };
}

namespace cxx::meta
{
  /*
   *
   */
  template <typename T>
  using quote = std::common_type<T>;

  /*
   *
   */
  template <bool... B>
  constexpr std::bool_constant<(B || ...)> any{};

  /*
   *
   */
  template <typename... T>
  struct type_list {
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

    /*
     *
     */
    template <typename U>
    static constexpr auto contains = any<std::is_same_v<U, T>...>;

    /*
     *
     */
    template <template <typename...> typename F>
    using apply = typename quote<F<T...>>::type;

    /*
     *
     */
    template <template <typename...> typename F>
    using find = typename decltype(type_list::find_impl<F, T...>())::type;
  };
}

namespace cxx
{
  /*
   *
   */
  template <typename... F>
  struct overload : F... {
    explicit constexpr overload(F... f) noexcept : F(std::move(f))... {}
    using F::operator()...;
  };

  template <typename... F>
  overload(F...)->overload<F...>;

  inline namespace literals
  {
    /*
     *
     */
    constexpr auto operator""_key(std::string::const_pointer p, std::string::size_type s) noexcept
    {
      struct sec {
      };
      struct impl {
        std::string::const_pointer const ptr;
        std::string::size_type const size;
        constexpr impl(sec, std::string::const_pointer x, std::string::size_type y)
            : ptr(x), size(y)
        {
        }
      };
      return impl(sec{}, p, s);
    }
  }

  /*
   *
   */
  using byte = std::byte;

  /*
   *
   */
  struct json {
    /*
     *
     */
    struct null_t {
      constexpr bool operator==(null_t) const noexcept { return true; }
      constexpr bool operator!=(null_t) const noexcept { return false; }
    };
    static constexpr null_t null{};

    /*
     *
     */
    using byte_stream = std::vector<cxx::byte>;

    /*
     *
     */
    using document = std::map<std::string, json>;

    /*
     *
     */
    using array = std::vector<json>;

    /*
     *
     */
    using alternatives = meta::
        type_list<document, std::int64_t, array, std::string, byte_stream, double, bool, null_t>;

    /*
     *
     */
    using key = decltype(""_key);

    template <typename T>
    static constexpr auto is_alternative = json::alternatives::contains<T>;

    /*
     *
     */
    template <typename T>
    using compatibile_alternative =
        std::conditional_t<json::is_alternative<std::decay_t<T>>,
                           std::decay_t<T>,
                           json::alternatives::find<traits::is_convertible_to<T>::template type>>;

    /*
     *
     */
    template <typename T>
    static constexpr bool is_compatibile =
        !std::is_same_v<json, std::decay_t<T>> &&
        (json::is_alternative<std::decay_t<T>> ||
         json::alternatives::apply<
             traits::is_convertible_to<std::decay_t<T>>::template any_of>::value);

    /*
     *
     */
    using object = json::alternatives::apply<std::variant>;

    object& to_object() noexcept { return storage; }
    object const& to_object() const noexcept { return storage; }

    /*
     *
     */
    template <typename T, typename = std::enable_if_t<json::is_compatibile<T>>>
    json(T&& t) noexcept(noexcept(json::compatibile_alternative<T>(std::forward<T>(t))))
        : storage(json::compatibile_alternative<T>(std::forward<T>(t)))
    {
    }

    json() noexcept = default;
    json(json const&) = default;
    json(json&&) noexcept = default;
    json& operator=(json const&) = default;
    json& operator=(json&&) noexcept = default;

    /*
     *
     */
    json(std::initializer_list<cxx::json::array::value_type>);
    json& operator=(std::initializer_list<cxx::json::array::value_type>);

    /*
     *
     */
    json(std::initializer_list<cxx::json::byte_stream::value_type>);
    json& operator=(std::initializer_list<cxx::json::byte_stream::value_type>);

    /*
     *
     */
    json(std::initializer_list<std::pair<json::key const, json>>);
    json& operator=(std::initializer_list<std::pair<json::key const, json>>);

    template <typename T, typename = std::enable_if_t<json::is_compatibile<T>>>
    json& operator=(T&& t) noexcept(noexcept(json::compatibile_alternative<T>(std::forward<T>(t))))
    {
      storage.emplace<json::compatibile_alternative<T>>(std::forward<T>(t));
      return *this;
    }

    /*
     *
     */
    json& operator[](std::string const&);
    json const& operator[](std::string const&) const;

    /*
     *
     */
    json& operator[](std::size_t);
    json const& operator[](std::size_t) const;

    /*
     *
     */
    std::size_t size() const noexcept;
    bool empty() const noexcept;

  private:
    object storage;
  };

  /*
   *
   */
  constexpr auto const to_object =
      overload([](json& x) -> json::object& { return x.to_object(); },
               [](json const& x) -> json::object const& { return x.to_object(); });

  /*
   *
   */
  template <typename T>
  constexpr auto const get =
      overload([](json const& x) -> decltype(auto) { return std::get<T>(to_object(x)); },
               [](json& x) -> decltype(auto) { return std::get<T>(to_object(x)); });

  /*
   *
   */
  constexpr auto const visit =
      overload([](auto&& f, json& x) -> decltype(
                   auto) { return std::visit(std::forward<decltype(f)>(f), to_object(x)); },
               [](auto&& f, json const& x) -> decltype(auto) {
                 return std::visit(std::forward<decltype(f)>(f), to_object(x));
               });

  /*
   *
   */
  template <typename T>
  constexpr auto const holds_alternative =
      [](json const& j) -> bool { return std::holds_alternative<T>(cxx::to_object(j)); };

  bool operator==(json const& lhs, json const& rhs) noexcept;
  bool operator!=(json const& lhs, json const& rhs) noexcept;

  /*
   *
   */
  template <typename T>
  auto operator==(json const& j, T const& rhs) noexcept
      -> std::enable_if_t<json::is_compatibile<T>, bool>;

  template <typename T>
  auto operator==(T const& lhs, json const& rhs) noexcept
      -> std::enable_if_t<json::is_compatibile<T>, bool>;

  template <typename T>
  auto operator!=(json const& lhs, T const& rhs) noexcept
      -> std::enable_if_t<json::is_compatibile<T>, bool>;

  template <typename T>
  auto operator!=(T const& lhs, json const& rhs) noexcept
      -> std::enable_if_t<json::is_compatibile<T>, bool>;
}

/*
 *
 */
template <typename T>
auto ::cxx::operator==(json const& j, T const& rhs) noexcept
    -> std::enable_if_t<json::is_compatibile<T>, bool>
{
  using type = std::conditional_t<json::is_alternative<T>, T, json::compatibile_alternative<T>>;
  auto const func = [&rhs](auto const& lhs) -> bool {
    if
      constexpr(std::is_same_v<decltype(lhs), type const&>) return lhs == rhs;
    else
      return false;
  };
  return cxx::visit(func, j);
}

template <typename T>
auto ::cxx::operator==(T const& lhs, json const& rhs) noexcept
    -> std::enable_if_t<json::is_compatibile<T>, bool>
{
  return rhs == lhs;
}

template <typename T>
auto ::cxx::operator!=(json const& lhs, T const& rhs) noexcept
    -> std::enable_if_t<json::is_compatibile<T>, bool>
{
  return !(lhs == rhs);
}

template <typename T>
auto ::cxx::operator!=(T const& lhs, json const& rhs) noexcept
    -> std::enable_if_t<json::is_compatibile<T>, bool>
{
  return !(lhs == rhs);
}
