#include <benchmark/benchmark.h>
#include "inc/cxx/json.hpp"

template <typename T>
static void cxx_json_assign_doc(benchmark::State& state)
{
  cxx::json json;
  for (auto _ : state) {
    json = T{};
    benchmark::DoNotOptimize(json);
    json = cxx::json::document();
    benchmark::DoNotOptimize(json);
  }
}

BENCHMARK_TEMPLATE(cxx_json_assign_doc, std::int64_t);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, bool);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, double);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, std::string);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, cxx::json::byte_stream);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, cxx::json::null_t);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, cxx::json::array);
BENCHMARK_TEMPLATE(cxx_json_assign_doc, cxx::json::document);
