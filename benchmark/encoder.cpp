#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
#include "inc/cxx/msgpack.hpp"
#include "test/utils.hpp"
using namespace cxx::literals;
using namespace test::literals;

template <typename T, typename Codec>
static void cxx_encode(benchmark::State& state)
{
  T const t{};
  for (auto _ : state) { benchmark::DoNotOptimize(Codec::encode(t)); }
}

template <typename Codec>
static void cxx_encode_non_empty_byte_stream(benchmark::State& state)
{
  cxx::json const json = "deadbeef"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(Codec::encode(json));
}

template <typename Codec>
static void cxx_encode_non_empty_array(benchmark::State& state)
{
  cxx::json const json = cxx::json::array({42, "lorem", true, cxx::json::null});
  for (auto _ : state) benchmark::DoNotOptimize(Codec::encode(json));
}

template <typename Codec>
static void cxx_encode_non_empty_dictionary(benchmark::State& state)
{
  cxx::json const json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, "dolor"},
      {"sit"_key, true},
      {"amet"_key, cxx::json::null}
      // clang-format on
  };
  for (auto _ : state) benchmark::DoNotOptimize(Codec::encode(json));
}

template <typename Codec>
static void cxx_encode_small_string(benchmark::State& state)
{
  cxx::json const json = "lorem";
  for (auto _ : state) benchmark::DoNotOptimize(Codec::encode(json));
}

template <typename Codec>
static void cxx_encode_long_string(benchmark::State& state)
{
  cxx::json const json = "ipsum dolor sit amet consectetur";
  for (auto _ : state) benchmark::DoNotOptimize(Codec::encode(json));
}

template <std::int64_t N>
using Int = std::integral_constant<std::int64_t, N>;

BENCHMARK_TEMPLATE(cxx_encode, Int<0x0>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x0>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x2>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x2>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x40000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x200000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x1000000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<0x8000000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x2>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x2>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x40000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x200000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x1000000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000000000>, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, Int<-0x8000000000>, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::array, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::array, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::byte_stream, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::byte_stream, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::dictionary, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::dictionary, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::null_t, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, cxx::json::null_t, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, double, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, double, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, std::false_type, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, std::false_type, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, std::string, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, std::string, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode, std::true_type, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode, std::true_type, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode_long_string, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode_long_string, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_array, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_array, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_byte_stream, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_byte_stream, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_dictionary, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode_non_empty_dictionary, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_encode_small_string, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_encode_small_string, cxx::msgpack);
