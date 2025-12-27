include(FetchContent)

# Google Benchmark can optionally build its own tests; we don't need them
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

FetchContent_MakeAvailable(benchmark)

add_executable(hybrid_search_bench
  bm_lower_bound.cpp
)

target_link_libraries(hybrid_search_bench
  PRIVATE
    hybrid_search::hybrid_search
    benchmark::benchmark
    benchmark::benchmark_main
)

target_compile_features(hybrid_search_bench PRIVATE cxx_std_20)
