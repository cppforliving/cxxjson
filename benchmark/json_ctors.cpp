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

static void cxx_json_boolean_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(true));
}
BENCHMARK(cxx_json_boolean_ctor);

static void cxx_json_null_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(cxx::null));
}
BENCHMARK(cxx_json_null_ctor);

static void cxx_json_double_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(3.14));
}
BENCHMARK(cxx_json_double_ctor);

static void cxx_json_string_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(std::string()));
}
BENCHMARK(cxx_json_string_ctor);

static void cxx_json_array_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(cxx::array()));
}
BENCHMARK(cxx_json_array_ctor);

static void cxx_json_document_ctor(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(cxx::json(cxx::document()));
}
BENCHMARK(cxx_json_document_ctor);
