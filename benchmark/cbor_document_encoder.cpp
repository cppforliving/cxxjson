#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
using namespace cxx::literals;

static void cxx_cbor_encode_empty_document(benchmark::State& state)
{
  cxx::json const json = cxx::document();
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_empty_document);

static void cxx_cbor_encode_non_empty_document(benchmark::State& state)
{
  cxx::json const json = {
      // clang-format off
      {"lorem"_key, 42},
      {"ipsum"_key, "dolor"},
      {"sit"_key, true},
      {"amet"_key, cxx::null}
      // clang-format on
  };
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::encode(json));
}
BENCHMARK(cxx_cbor_encode_non_empty_document);
