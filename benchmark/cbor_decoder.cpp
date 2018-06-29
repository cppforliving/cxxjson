#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

static void cxx_cbor_decode_positive_integers(benchmark::State& state)
{
  auto const bytes = cxx::cbor::encode(state.range(0));
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_positive_integers)->RangeMultiplier(8)->Range(2, 1 << 30)->Complexity();

static void cxx_cbor_decode_negative_integers(benchmark::State& state)
{
  auto const bytes = cxx::cbor::encode(-state.range(0));
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_negative_integers)->RangeMultiplier(8)->Range(2, 1 << 30)->Complexity();

static void cxx_cbor_decode_byte_stream(benchmark::State& state)
{
  auto const bytes = "5803112233"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_byte_stream);
