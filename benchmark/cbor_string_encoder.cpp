#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"

static void cxx_cbor_encode_small_string(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode("lorem"));
}
BENCHMARK(cxx_cbor_encode_small_string);

static void cxx_cbor_encode_long_string(benchmark::State& state)
{
  for (auto _ : state)
    benchmark::DoNotOptimize(cxx::cbor::encode("ipsum dolor sit amet consectetur"));
}
BENCHMARK(cxx_cbor_encode_long_string);
