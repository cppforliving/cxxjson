#include <benchmark/benchmark.h>
#include "inc/cxx/cbor.hpp"
#include "inc/cxx/msgpack.hpp"
#include "test/utils.hpp"

using namespace cxx::literals;
using namespace test::literals;

template <typename Codec>
static void cxx_decode_positive_integers(benchmark::State& state)
{
  auto const bytes = Codec::encode(state.range(0));
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_negative_integers(benchmark::State& state)
{
  auto const bytes = Codec::encode(-state.range(0));
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_byte_stream(benchmark::State& state)
{
  auto const bytes = "5803112233"_hex;
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_unicode_string(benchmark::State& state)
{
  auto const bytes = Codec::encode("ipsum dolor sit amet consectetur");
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_array(benchmark::State& state)
{
  auto const bytes = Codec::encode({0, {"lorem", 0xff}, "ipsum"});
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_dictionary(benchmark::State& state)
{
  auto const bytes = Codec::encode({"lorem"_key >> "ipsum", "dolor"_key >> 42});
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_bool(benchmark::State& state)
{
  auto const bytes = Codec::encode(true);
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_null(benchmark::State& state)
{
  auto const bytes = Codec::encode(cxx::json::null);
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

template <typename Codec>
static void cxx_decode_double(benchmark::State& state)
{
  auto const bytes = Codec::encode(3.14);
  for (auto _ : state) benchmark::DoNotOptimize(Codec::decode(bytes));
}

BENCHMARK_TEMPLATE(cxx_decode_array, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_array, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_bool, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_bool, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_byte_stream, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_byte_stream, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_dictionary, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_dictionary, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_double, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_double, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_negative_integers, cxx::cbor)
    ->RangeMultiplier(8)
    ->Range(2, 1 << 30)
    ->Complexity();
BENCHMARK_TEMPLATE(cxx_decode_negative_integers, cxx::msgpack)
    ->RangeMultiplier(8)
    ->Range(2, 1 << 30)
    ->Complexity();
BENCHMARK_TEMPLATE(cxx_decode_null, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_null, cxx::msgpack);
BENCHMARK_TEMPLATE(cxx_decode_positive_integers, cxx::cbor)
    ->RangeMultiplier(8)
    ->Range(2, 1 << 30)
    ->Complexity();
BENCHMARK_TEMPLATE(cxx_decode_positive_integers, cxx::msgpack)
    ->RangeMultiplier(8)
    ->Range(2, 1 << 30)
    ->Complexity();
BENCHMARK_TEMPLATE(cxx_decode_unicode_string, cxx::cbor);
BENCHMARK_TEMPLATE(cxx_decode_unicode_string, cxx::msgpack);
