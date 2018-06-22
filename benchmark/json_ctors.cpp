#include <benchmark/benchmark.h>
#include "inc/cxx/json.hpp"


static void cxx_json_default_ctor(benchmark::State& state)
{
  for(auto _: state)
    benchmark::DoNotOptimize(cxx::json());
}

BENCHMARK(cxx_json_default_ctor);
