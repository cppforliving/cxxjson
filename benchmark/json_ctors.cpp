#include <benchmark/benchmark.h>
#include "inc/cxx/json.hpp"

static void cxx_json_default_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json());
}
BENCHMARK(cxx_json_default_ctor);

static void cxx_json_copy_ctor(benchmark::State& state)
{
  cxx::json const orig;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(orig));
}
BENCHMARK(cxx_json_copy_ctor);

static void cxx_json_integer_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(42));
}
BENCHMARK(cxx_json_integer_ctor);

static void cxx_json_string_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json("lorem"));
}
BENCHMARK(cxx_json_string_ctor);
