#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <random>
#include <span>
#include <type_traits>
#include <vector>
#include <forward_list>

// Your library
#include "../include/Boundcraft/boundcraft.hpp"

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

namespace {

template <class It>
std::size_t idx_of(It first, It it)
{
    return static_cast<std::size_t>(std::distance(first, it));
}

// Compare Boundcraft to std::lower_bound (iterator interface)
template <class Policy, class Container, class V, class Comp>
void check_lb_iter(Container& c, const V& value, Comp comp)
{
    boundcraft::searcher<Policy> s;

    auto got = s.lower_bound(c.begin(), c.end(), value, comp);
    auto exp = std::lower_bound(c.begin(), c.end(), value, comp);

    ASSERT_EQ(idx_of(c.begin(), got), idx_of(c.begin(), exp));
}

// Compare Boundcraft to std::lower_bound (pointer interface)
template <class Policy, class T, class V, class Comp>
void check_lb_ptr(std::vector<T>& v, const V& value, Comp comp)
{
    boundcraft::searcher<Policy> s;

    T* first = v.data();
    T* last  = v.data() + v.size();

    T* got = s.lower_bound(first, last, value, comp);
    auto exp_it = std::lower_bound(v.begin(), v.end(), value, comp);

    ASSERT_EQ(static_cast<std::size_t>(got - first), idx_of(v.begin(), exp_it));
}

// Compare Boundcraft to std::lower_bound (span interface)
template <class Policy, class T, class V, class Comp>
void check_lb_span(std::vector<T>& v, const V& value, Comp comp)
{
    boundcraft::searcher<Policy> s;

    std::span<T> sp(v.data(), v.size());
    T* got = s.lower_bound(sp, value, comp);
    auto exp_it = std::lower_bound(v.begin(), v.end(), value, comp);

    ASSERT_EQ(static_cast<std::size_t>(got - v.data()), idx_of(v.begin(), exp_it));
}

template <class T>
std::vector<T> make_sorted_with_dups(std::size_t n, int minv, int maxv, std::mt19937& rng)
{
    std::uniform_int_distribution<int> dist(minv, maxv);
    std::vector<T> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) v.push_back(static_cast<T>(dist(rng)));
    std::sort(v.begin(), v.end());
    return v;
}

template <class T>
std::vector<T> make_sorted_desc_with_dups(std::size_t n, int minv, int maxv, std::mt19937& rng)
{
    auto v = make_sorted_with_dups<T>(n, minv, maxv, rng);
    std::sort(v.begin(), v.end(), std::greater<>{});
    return v;
}

} // namespace

// ------------------------------------------------------------
// Policies under test
// ------------------------------------------------------------

namespace policies {

// Search policies
using stdbin = boundcraft::policy::standard_binary;

// Hybrid search policies
using hyb1  = boundcraft::policy::hybrid<1>;
using hyb4  = boundcraft::policy::hybrid<4>;
using hyb16 = boundcraft::policy::hybrid<16>;
using hyb64 = boundcraft::policy::hybrid<64>;

// Gallop start strategies
namespace gallop = boundcraft::policy::gallop;

using g_front  = gallop::start_front;
using g_back   = gallop::start_back;
using g_middle = gallop::start_middle;
using g_last3  = gallop::start_last_searched<3>;
using g_last0  = gallop::start_last_searched<0>;

// Galloping policy wrapper (RA-only support in the library)
template <class SearchPolicy, class StartPolicy>
using galloping = boundcraft::policy::galloping<SearchPolicy, StartPolicy>;

// A few galloping combos
using g_stdbin_front   = galloping<stdbin, g_front>;
using g_stdbin_back    = galloping<stdbin, g_back>;
using g_stdbin_middle  = galloping<stdbin, g_middle>;
using g_stdbin_last3   = galloping<stdbin, g_last3>;
using g_hyb16_middle   = galloping<hyb16, g_middle>;
using g_hyb64_front    = galloping<hyb64, g_front>;

} // namespace policies

// ------------------------------------------------------------
// Core deterministic test cases (ascending) - RA containers (vector)
// ------------------------------------------------------------

template <class Policy>
class LowerBoundAscendingDeterministic : public ::testing::Test {};

using AscDetPolicies = ::testing::Types<
    policies::stdbin,
    policies::hyb1, policies::hyb4, policies::hyb16, policies::hyb64,
    // Galloping policies are RA-only -> tested here with std::vector
    policies::g_stdbin_front, policies::g_stdbin_back, policies::g_stdbin_middle, policies::g_stdbin_last3,
    policies::g_hyb16_middle, policies::g_hyb64_front
>;
TYPED_TEST_SUITE(LowerBoundAscendingDeterministic, AscDetPolicies);

TYPED_TEST(LowerBoundAscendingDeterministic, EmptyRangeReturnsBegin)
{
    std::vector<int> v;
    check_lb_iter<TypeParam>(v, 10, std::less<>{});
    check_lb_ptr<TypeParam>(v, 10, std::less<>{});
    check_lb_span<TypeParam>(v, 10, std::less<>{});
}

TYPED_TEST(LowerBoundAscendingDeterministic, SingleElement)
{
    std::vector<int> v{5};

    check_lb_iter<TypeParam>(v, 4, std::less<>{});
    check_lb_iter<TypeParam>(v, 5, std::less<>{});
    check_lb_iter<TypeParam>(v, 6, std::less<>{});

    check_lb_ptr<TypeParam>(v, 4, std::less<>{});
    check_lb_ptr<TypeParam>(v, 5, std::less<>{});
    check_lb_ptr<TypeParam>(v, 6, std::less<>{});

    check_lb_span<TypeParam>(v, 4, std::less<>{});
    check_lb_span<TypeParam>(v, 5, std::less<>{});
    check_lb_span<TypeParam>(v, 6, std::less<>{});
}

TYPED_TEST(LowerBoundAscendingDeterministic, MultipleElementsBasicPositions)
{
    std::vector<int> v{1, 3, 5, 7, 9};

    check_lb_iter<TypeParam>(v, 0, std::less<>{});
    check_lb_iter<TypeParam>(v, 1, std::less<>{});
    check_lb_iter<TypeParam>(v, 2, std::less<>{});
    check_lb_iter<TypeParam>(v, 9, std::less<>{});
    check_lb_iter<TypeParam>(v, 10, std::less<>{});

    check_lb_ptr<TypeParam>(v, 0, std::less<>{});
    check_lb_ptr<TypeParam>(v, 2, std::less<>{});
    check_lb_ptr<TypeParam>(v, 10, std::less<>{});

    check_lb_span<TypeParam>(v, 0, std::less<>{});
    check_lb_span<TypeParam>(v, 2, std::less<>{});
    check_lb_span<TypeParam>(v, 10, std::less<>{});
}

TYPED_TEST(LowerBoundAscendingDeterministic, HandlesDuplicatesReturnsFirstDuplicate)
{
    std::vector<int> v{1, 2, 2, 2, 3, 4};

    // lower_bound(2) should return index 1
    boundcraft::searcher<TypeParam> s;
    auto got = s.lower_bound(v.begin(), v.end(), 2, std::less<>{});
    ASSERT_EQ(idx_of(v.begin(), got), 1u);

    // Compare to oracle for a few values
    check_lb_iter<TypeParam>(v, 2, std::less<>{});
    check_lb_iter<TypeParam>(v, 3, std::less<>{});
    check_lb_iter<TypeParam>(v, 0, std::less<>{});
    check_lb_iter<TypeParam>(v, 5, std::less<>{});

    check_lb_ptr<TypeParam>(v, 2, std::less<>{});
    check_lb_span<TypeParam>(v, 2, std::less<>{});
}

// ------------------------------------------------------------
// Deterministic descending comparator tests - RA containers (vector)
// ------------------------------------------------------------

template <class Policy>
class LowerBoundDescendingDeterministic : public ::testing::Test {};

using DescDetPolicies = ::testing::Types<
    policies::stdbin,
    policies::hyb16,
    // Galloping policies are RA-only -> tested here with std::vector
    policies::g_stdbin_middle,
    policies::g_hyb16_middle
>;
TYPED_TEST_SUITE(LowerBoundDescendingDeterministic, DescDetPolicies);

TYPED_TEST(LowerBoundDescendingDeterministic, WorksWithGreaterComparatorOnDescendingSortedData)
{
    std::vector<int> v{9, 7, 7, 5, 3, 1};
    auto comp = std::greater<>{};

    check_lb_iter<TypeParam>(v, 10, comp);
    check_lb_iter<TypeParam>(v, 9, comp);
    check_lb_iter<TypeParam>(v, 8, comp);
    check_lb_iter<TypeParam>(v, 7, comp);
    check_lb_iter<TypeParam>(v, 0, comp);

    check_lb_ptr<TypeParam>(v, 7, comp);
    check_lb_span<TypeParam>(v, 7, comp);
}

// ------------------------------------------------------------
// Forward-iterator coverage (std::forward_list) - NO galloping here
// Galloping is RANDOM-ACCESS ONLY by design.
// ------------------------------------------------------------

template <class Policy>
class LowerBoundForwardIteratorCoverage : public ::testing::Test {};

using ForwardPolicies = ::testing::Types<
    policies::stdbin,
    policies::hyb16
>;
TYPED_TEST_SUITE(LowerBoundForwardIteratorCoverage, ForwardPolicies);

TYPED_TEST(LowerBoundForwardIteratorCoverage, ForwardListMatchesStdLowerBound)
{
    std::forward_list<int> fl = {1, 2, 2, 4, 7, 9};
    boundcraft::searcher<TypeParam> s;

    for (int q : {0, 1, 2, 3, 4, 10})
    {
        auto got = s.lower_bound(fl.begin(), fl.end(), q, std::less<>{});
        auto exp = std::lower_bound(fl.begin(), fl.end(), q, std::less<>{});
        ASSERT_EQ(std::distance(fl.begin(), got), std::distance(fl.begin(), exp));
    }
}

// ------------------------------------------------------------
// Randomized property tests
// ------------------------------------------------------------

template <class Policy>
class LowerBoundRandomized : public ::testing::Test {};

using RandPolicies = ::testing::Types<
    policies::stdbin,
    policies::hyb4,
    policies::hyb16,
    // Galloping policies are RA-only -> these tests use std::vector
    policies::g_stdbin_front,
    policies::g_stdbin_middle,
    policies::g_stdbin_last3,
    policies::g_hyb16_middle
>;
TYPED_TEST_SUITE(LowerBoundRandomized, RandPolicies);

TYPED_TEST(LowerBoundRandomized, RandomSortedAscendingMatchesStdLowerBoundManyQueries)
{
    std::mt19937 rng(123456u);

    for (int trial = 0; trial < 25; ++trial)
    {
        auto v = make_sorted_with_dups<int>(500, -200, 200, rng);

        std::uniform_int_distribution<int> qdist(-250, 250);
        for (int i = 0; i < 200; ++i)
        {
            int q = qdist(rng);
            check_lb_iter<TypeParam>(v, q, std::less<>{});
        }

        for (int i = 0; i < 25; ++i)
        {
            int q = qdist(rng);
            check_lb_ptr<TypeParam>(v, q, std::less<>{});
            check_lb_span<TypeParam>(v, q, std::less<>{});
        }
    }
}

TYPED_TEST(LowerBoundRandomized, RandomSortedDescendingMatchesStdLowerBoundManyQueries)
{
    std::mt19937 rng(98765u);
    auto comp = std::greater<>{};

    for (int trial = 0; trial < 15; ++trial)
    {
        auto v = make_sorted_desc_with_dups<int>(600, -300, 300, rng);

        std::uniform_int_distribution<int> qdist(-400, 400);
        for (int i = 0; i < 200; ++i)
        {
            int q = qdist(rng);
            check_lb_iter<TypeParam>(v, q, comp);
        }
    }
}

// ------------------------------------------------------------
// Gallop start-point sanity (RA-only test)
// ------------------------------------------------------------

TEST(GallopStartPolicy, StartLastSearchedBehavesCorrectly)
{
    using P = policies::g_stdbin_last3;

    std::vector<int> v{1, 2, 4, 8, 16, 32, 64, 128};

    boundcraft::searcher<P> s;
    auto got = s.lower_bound(v.begin(), v.end(), 10, std::less<>{});
    auto exp = std::lower_bound(v.begin(), v.end(), 10, std::less<>{});

    ASSERT_EQ(idx_of(v.begin(), got), idx_of(v.begin(), exp));
}
