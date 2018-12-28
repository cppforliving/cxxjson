#pragma once
#include <memory>

namespace cxx
{
  template <typename T>
  struct output_parameter {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    constexpr explicit output_parameter(reference r) noexcept : ptr(std::addressof(r)) {}
    output_parameter() = delete;
    output_parameter(T&&) = delete;
    output_parameter(T const&) = delete;
    output_parameter(output_parameter const&) = delete;
    output_parameter(output_parameter&&) = delete;
    output_parameter& operator=(output_parameter const&) = delete;
    output_parameter& operator=(output_parameter&&) = delete;

    template <typename U>
    output_parameter& operator=(U u)
    {
      *ptr = std::move(u);
      return *this;
    }

    reference operator*() noexcept { return *ptr; }
    pointer operator->() noexcept { return ptr; }

  private:
    pointer ptr;
  };

  template <typename>
  struct is_output_parameter : std::false_type {
  };

  template <typename T>
  struct is_output_parameter<output_parameter<T>> : std::true_type {
  };

  inline auto const by_ref = [](auto& ref) {
    using param = std::remove_reference_t<decltype(ref)>;
    if constexpr (is_output_parameter<param>::value)
    { return output_parameter<typename param::value_type>(*ref); }
    else
    {
      return output_parameter<param>(ref);
    }
  };
} // namespace cxx
