#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"

static void cxx_cbor_encode_empty_array(benchmark::State& state)
{
  cxx::json const json = cxx::array();
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_empty_array);

static void cxx_cbor_encode_non_empty_array(benchmark::State& state)
{
  cxx::json const json = cxx::array({42, "lorem", true, cxx::null});
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_non_empty_array);
