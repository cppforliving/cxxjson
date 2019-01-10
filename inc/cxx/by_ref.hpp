#pragma once
#include <memory>

namespace cxx
{
  template <typename T>
  struct by_ref {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_const_v<T>);

    by_ref() = delete;
    by_ref(T&&) = delete;
    by_ref(T const&) = delete;
    by_ref(by_ref&&) = delete;
    by_ref& operator=(by_ref const&) = delete;
    by_ref& operator=(by_ref&&) = delete;

    constexpr explicit by_ref(T& r) noexcept : ptr(std::addressof(r)) {}
    constexpr explicit by_ref(by_ref const&) = default;
    ~by_ref() = default;
    template <typename U>
    constexpr by_ref& operator=(U u)
    {
      *ptr = std::move(u);
      return *this;
    }

    constexpr T& get() noexcept { return *ptr; }
    constexpr T* operator->() noexcept { return ptr; }
    constexpr operator T const&() const noexcept { return *ptr; }

  private:
    T* ptr;
  };

  template <typename T>
  by_ref(T&)->by_ref<T>;

  template <typename T>
  by_ref(by_ref<T>&)->by_ref<T>;
} // namespace cxx
