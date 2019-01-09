#pragma once
#include <memory>

namespace cxx
{
  template <typename T>
  struct output_parameter {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);

    constexpr explicit output_parameter(T& r) noexcept : ptr(std::addressof(r)) {}
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

    T& get() noexcept { return *ptr; }
    constexpr T* data() noexcept { return ptr; }
    constexpr T* operator->() noexcept { return data(); }
    constexpr operator T const&() const noexcept { return *ptr; }

    template <typename U>
    friend output_parameter<U> make_output_parameter(output_parameter<U>&) noexcept;

    template <typename U>
    friend output_parameter<U> make_output_parameter(U& u) noexcept;

  private:
    T* ptr;
  };

  template <typename U>
  output_parameter<U> make_output_parameter(output_parameter<U>& u) noexcept
  {
    return output_parameter<U>(*u.ptr);
  }

  template <typename U>
  output_parameter<U> make_output_parameter(U& u) noexcept
  {
    return output_parameter<U>(u);
  }

  inline auto const by_ref = [](auto& ref) { return cxx::make_output_parameter(ref); };
} // namespace cxx
