#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

static void cxx_cbor_decode_init_byte_integer(benchmark::State& state)
{
  auto const bytes = "10"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_init_byte_integer);

static void cxx_cbor_decode_one_additional_byte_integer(benchmark::State& state)
{
  auto const bytes = "1818"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_one_additional_byte_integer);

static void cxx_cbor_decode_two_additional_bytes_integer(benchmark::State& state)
{
  auto const bytes = "191234"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_two_additional_bytes_integer);

static void cxx_cbor_decode_four_additional_bytes_integer(benchmark::State& state)
{
  auto const bytes = "1a12345678"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(cxx::cbor::decode(bytes));
}
BENCHMARK(cxx_cbor_decode_four_additional_bytes_integer);
