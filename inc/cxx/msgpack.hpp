#pragma once
#include <cxx/json.hpp>
#include <cxx/by_ref.hpp>

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
    static json::byte_stream encode(json const&);

    /*
     *
     */
    static json decode(json::byte_stream const&);

    /*
     *
     */
    static json decode(cxx::output_parameter<json::byte_view>);
  };
} // namespace cxx
