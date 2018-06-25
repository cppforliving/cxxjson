#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"

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
