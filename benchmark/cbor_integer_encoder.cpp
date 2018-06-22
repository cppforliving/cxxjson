#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"

static void cxx_cbor_encode_positive_intiger(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(state.range(0)));
  state.SetComplexityN(state.range(0));
}
BENCHMARK(cxx_cbor_encode_positive_intiger)->RangeMultiplier(8)->Range(0x2, 0x1fffffff);

static void cxx_cbor_encode_negative_intiger(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(state.range(0)));
  state.SetComplexityN(state.range(0));
}
BENCHMARK(cxx_cbor_encode_negative_intiger)
    ->Arg(-2)
    ->Arg(-8)
    ->Arg(-0x10)
    ->Arg(-0x1f)
    ->Arg(-0x1fff)
    ->Arg(-0x1abbccdd);
