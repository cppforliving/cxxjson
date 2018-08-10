#pragma once
#include <cxx/json.hpp>

namespace cxx
{
  /*
   *
   */
  struct msgpack {
    /*
     *
     */
    struct error : virtual std::runtime_error {
      using std::runtime_error::runtime_error;
      virtual ~error() = default;
    };

    /*
     *
     */
    struct truncation_error : virtual error {
      using error::error;
      virtual ~truncation_error() = default;
    };

    /*
     *
     */
    struct unsupported : virtual error {
      using error::error;
      virtual ~unsupported() = default;
    };

    /*
     *
     */
    struct data_error : virtual error {
      using error::error;
      virtual ~data_error() = default;
    };

    /*
     *
     */
    constexpr static std::size_t const max_size = 0xffffffff;

    /*
     *
     */
    static json::byte_stream encode(json const&) noexcept;

    /*
     *
     */
    static json decode(json::byte_stream const&);

    /*
     *
     */
    static json decode(json::byte_view&);
  };
} // namespace cxx
