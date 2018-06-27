#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
#include "test/utils.hpp"
using namespace cxx::literals;
using namespace test::literals;

template <typename T>
static void cxx_cbor_encode(benchmark::State& state)
{
  T const t{};
  for (auto _ : state) {
    benchmark::DoNotOptimize(cxx::cbor::encode(t));
  }
}

template <std::int64_t N>
using Int = std::integral_constant<std::int64_t, N>;

BENCHMARK_TEMPLATE(cxx_cbor_encode, cxx::json::null_t);
BENCHMARK_TEMPLATE(cxx_cbor_encode, std::true_type);
BENCHMARK_TEMPLATE(cxx_cbor_encode, std::false_type);
BENCHMARK_TEMPLATE(cxx_cbor_encode, double);
BENCHMARK_TEMPLATE(cxx_cbor_encode, std::string);
BENCHMARK_TEMPLATE(cxx_cbor_encode, cxx::json::byte_stream);
BENCHMARK_TEMPLATE(cxx_cbor_encode, cxx::json::array);
BENCHMARK_TEMPLATE(cxx_cbor_encode, cxx::json::document);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x0>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x2>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x8>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x40>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x200>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x1000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x8000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x40000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x200000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x1000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x8000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x40000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x200000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x1000000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<0x8000000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x2>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x8>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x40>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x200>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x1000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x8000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x40000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x200000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x1000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x8000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x40000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x200000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x1000000000>);
BENCHMARK_TEMPLATE(cxx_cbor_encode, Int<-0x8000000000>);

static void cxx_cbor_encode_non_empty_byte_stream(benchmark::State& state)
{
  cxx::json const json = "deadbeef"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_non_empty_byte_stream);

static void cxx_cbor_encode_non_empty_array(benchmark::State& state)
{
  cxx::json const json = cxx::json::array({42, "lorem", true, cxx::json::null});
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_non_empty_array);

static void cxx_cbor_encode_non_empty_document(benchmark::State& state)
{
  cxx::json const json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, "dolor"},
      {"sit"_key, true},
      {"amet"_key, cxx::json::null}
      // clang-format on
  };
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_non_empty_document);

static void cxx_cbor_encode_small_string(benchmark::State& state)
{
  cxx::json const json = "lorem";
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_small_string);

static void cxx_cbor_encode_long_string(benchmark::State& state)
{
  cxx::json const json = "ipsum dolor sit amet consectetur";
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_long_string);
