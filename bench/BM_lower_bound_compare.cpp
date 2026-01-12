#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include <boundcraft/boundcraft.hpp>

namespace {

static std::vector<int> make_sorted_unique(std::size_t n, int start = 0, int step = 2) {
    std::vector<int> v;
    v.reserve(n);
    int x = start;
    for (std::size_t i = 0; i < n; ++i) {
        v.push_back(x);
        x += step;
    }
    return v;
}

enum class QueryPattern {
    UniformRandom,
    MostlyHits,
    MostlyMisses,
    NearFront,
    NearBack
};

static int make_query(std::mt19937& rng, const std::vector<int>& data, QueryPattern pat) {
    const std::size_t n = data.size();
    if (n == 0) return 0;

    std::uniform_int_distribution<std::size_t> idx_dist(0, n - 1);
    std::uniform_int_distribution<int> coin(0, 99);

    auto pick_hit = [&] {
        return data[idx_dist(rng)];
    };

    auto pick_miss = [&] {
        std::uniform_int_distribution<int> miss_dist(data.front(), data.back());
        int x = miss_dist(rng);
        return (x % 2 == 0) ? (x + 1) : x; // odds are misses for step=2
    };

    switch (pat) {
        case QueryPattern::UniformRandom:
            return (coin(rng) < 50) ? pick_hit() : pick_miss();
        case QueryPattern::MostlyHits:
            return (coin(rng) < 90) ? pick_hit() : pick_miss();
        case QueryPattern::MostlyMisses:
            return (coin(rng) < 90) ? pick_miss() : pick_hit();
        case QueryPattern::NearFront: {
            std::size_t hi = std::max<std::size_t>(1, n / 16);
            std::uniform_int_distribution<std::size_t> front_dist(0, hi - 1);
            int base = data[front_dist(rng)];
            std::uniform_int_distribution<int> jitter(-3, 3);
            return base + jitter(rng);
        }
        case QueryPattern::NearBack: {
            std::size_t lo = (n > 1) ? (n - std::max<std::size_t>(1, n / 16)) : 0;
            std::uniform_int_distribution<std::size_t> back_dist(lo, n - 1);
            int base = data[back_dist(rng)];
            std::uniform_int_distribution<int> jitter(-3, 3);
            return base + jitter(rng);
        }
    }
    return data[idx_dist(rng)];
}

template <class LB>
static void run_bench(benchmark::State& state, QueryPattern pat, LB&& lb) {
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    auto data = make_sorted_unique(n);

    std::mt19937 rng(123456u);
    std::vector<int> queries;
    queries.resize(4096);
    for (auto& q : queries) q = make_query(rng, data, pat);

    std::size_t qi = 0;
    std::size_t sink = 0;

    for (auto _ : state) {
        const int key = queries[qi++ & (queries.size() - 1)];

        auto it = lb(data, key);

        // FIX: cbegin() matches const_iterator
        sink += static_cast<std::size_t>(std::distance(data.cbegin(), it));

        benchmark::DoNotOptimize(sink);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

} // namespace

// std::lower_bound
static void BM_std_lower_bound_uniform(benchmark::State& state) {
    run_bench(state, QueryPattern::UniformRandom,
             [](const std::vector<int>& data, int key) {
                 return std::lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_std_lower_bound_hits(benchmark::State& state) {
    run_bench(state, QueryPattern::MostlyHits,
             [](const std::vector<int>& data, int key) {
                 return std::lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_std_lower_bound_misses(benchmark::State& state) {
    run_bench(state, QueryPattern::MostlyMisses,
             [](const std::vector<int>& data, int key) {
                 return std::lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_std_lower_bound_front(benchmark::State& state) {
    run_bench(state, QueryPattern::NearFront,
             [](const std::vector<int>& data, int key) {
                 return std::lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_std_lower_bound_back(benchmark::State& state) {
    run_bench(state, QueryPattern::NearBack,
             [](const std::vector<int>& data, int key) {
                 return std::lower_bound(data.begin(), data.end(), key);
             });
}

// BoundCraft standard
static void BM_bc_standard_uniform(benchmark::State& state) {
    using Policy = boundcraft::policy::standard_binary;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::UniformRandom,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_bc_standard_hits(benchmark::State& state) {
    using Policy = boundcraft::policy::standard_binary;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::MostlyHits,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_bc_standard_misses(benchmark::State& state) {
    using Policy = boundcraft::policy::standard_binary;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::MostlyMisses,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_bc_standard_front(benchmark::State& state) {
    using Policy = boundcraft::policy::standard_binary;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::NearFront,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}
static void BM_bc_standard_back(benchmark::State& state) {
    using Policy = boundcraft::policy::standard_binary;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::NearBack,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}

// hybrids
static void BM_bc_hybrid16_uniform(benchmark::State& state) {
    using Policy = boundcraft::policy::hybrid<16>;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::UniformRandom,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}

static void BM_bc_hybrid64_uniform(benchmark::State& state) {
    using Policy = boundcraft::policy::hybrid<64>;
    boundcraft::searcher<Policy> s;
    run_bench(state, QueryPattern::UniformRandom,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}

// galloping examples (update types if your policy names differ)
static void BM_bc_gallop_std_front_uniform(benchmark::State& state) {
    using Policy = boundcraft::policy::galloping<
        boundcraft::policy::standard_binary,
        boundcraft::policy::gallop::start_front>;
    boundcraft::searcher<Policy> s;

    run_bench(state, QueryPattern::UniformRandom,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}

static void BM_bc_gallop_hybrid16_front_uniform(benchmark::State& state) {
    using Policy = boundcraft::policy::galloping<
        boundcraft::policy::hybrid<16>,
        boundcraft::policy::gallop::start_front>;
    boundcraft::searcher<Policy> s;

    run_bench(state, QueryPattern::UniformRandom,
             [&](const std::vector<int>& data, int key) {
                 return s.lower_bound(data.begin(), data.end(), key);
             });
}

BENCHMARK(BM_std_lower_bound_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_std_lower_bound_hits)   ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_std_lower_bound_misses) ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_std_lower_bound_front)  ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_std_lower_bound_back)   ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);

BENCHMARK(BM_bc_standard_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_standard_hits)   ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_standard_misses) ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_standard_front)  ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_standard_back)   ->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);

BENCHMARK(BM_bc_hybrid16_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_hybrid64_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);

BENCHMARK(BM_bc_gallop_std_front_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);
BENCHMARK(BM_bc_gallop_hybrid16_front_uniform)->Arg(1<<10)->Arg(1<<14)->Arg(1<<18)->Arg(1<<22);

BENCHMARK_MAIN();
