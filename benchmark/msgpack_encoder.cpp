#include <benchmark/benchmark.h>
#include "inc/cxx/msgpack.hpp"

static void cxx_msgpack_encode(benchmark::State& state)
{
  auto const x = state.range(0);
  for (auto _ : state) { benchmark::DoNotOptimize(cxx::msgpack::encode(x)); }
}

BENCHMARK(cxx_msgpack_encode)->RangeMultiplier(2)->Range(8, 0xffffffff);
