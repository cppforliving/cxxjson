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

template <typename T>
static void cxx_json_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(T{}));
}

BENCHMARK_TEMPLATE(cxx_json_ctor, std::int64_t);
BENCHMARK_TEMPLATE(cxx_json_ctor, bool);
BENCHMARK_TEMPLATE(cxx_json_ctor, double);
BENCHMARK_TEMPLATE(cxx_json_ctor, std::string);
BENCHMARK_TEMPLATE(cxx_json_ctor, cxx::json::null_t);
BENCHMARK_TEMPLATE(cxx_json_ctor, cxx::json::array);
BENCHMARK_TEMPLATE(cxx_json_ctor, cxx::json::document);
