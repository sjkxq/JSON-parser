#include <benchmark/benchmark.h>
#include <fstream>
#include <sjkxq_json/json.hpp>
#include <sstream>

// Test data constants
const std::string SMALL_JSON = R"({"key":"value"})";

using namespace sjkxq_json;

static void BM_ParseSmallJson(benchmark::State& state) {
    for (auto _ : state) {
        Value v = parse(SMALL_JSON);
        benchmark::DoNotOptimize(v);
    }
    state.SetBytesProcessed(state.iterations() * SMALL_JSON.size());
}
BENCHMARK(BM_ParseSmallJson);

static void BM_ParseLargeJson(benchmark::State& state) {
    std::ifstream file("test_data/large.json");
    if (!file.is_open()) {
        state.SkipWithError("Failed to open test_data/large.json");
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string json = buffer.str();

    for (auto _ : state) {
        Value v = parse(json);
        benchmark::DoNotOptimize(v);
    }
    state.SetBytesProcessed(state.iterations() * json.size());
}
BENCHMARK(BM_ParseLargeJson);

static void BM_Stringify(benchmark::State& state) {
    Value v;
    v["name"] = "test";
    v["value"] = 42;
    Value::Array arr;
    arr.emplace_back(1);
    arr.emplace_back(2);
    arr.emplace_back(3);
    v["items"] = Value(arr);

    // Pre-compute the size once
    const size_t json_size = stringify(v).size();

    for (auto _ : state) {
        std::string result = stringify(v);
        benchmark::DoNotOptimize(result);
    }
    state.SetBytesProcessed(state.iterations() * json_size);
}
BENCHMARK(BM_Stringify);

BENCHMARK_MAIN();