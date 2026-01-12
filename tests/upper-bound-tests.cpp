#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <span>
#include <type_traits>
#include <vector>

#include <boundcraft/searcher.hpp>

namespace {

struct Elem {
    int key;
    int payload;
};

inline bool operator<(const Elem& a, const Elem& b) { return a.key < b.key; }

// NOTE:
// Your upper_bound_strict() is constrained with something like
// std::indirect_strict_weak_order<Comp, It, const V*>
// which ends up requiring the comparator to work for V vs V too.
// Therefore we provide int/int as well.
struct ElemKeyStrictLess {
    // Elem vs key
    bool operator()(const Elem& e, int k) const noexcept { return e.key < k; }
    // key vs Elem
    bool operator()(int k, const Elem& e) const noexcept { return k < e.key; }
    // Elem vs Elem
    bool operator()(const Elem& a, const Elem& b) const noexcept { return a.key < b.key; }
    // key vs key  <-- this is what your error was complaining about
    bool operator()(int a, int b) const noexcept { return a < b; }
};

// One-way comparator for upper_bound (value, elem)
inline constexpr auto one_way_upper_comp = [](int k, const Elem& e) noexcept {
    return k < e.key;
};

template <class Policy>
using Searcher = boundcraft::searcher<Policy>;

// EDIT THIS LIST:
using PoliciesUnderTest = ::testing::Types<boundcraft::policy::standard_binary>;

template <class Policy>
class UpperBoundTests : public ::testing::Test {
public:
    using policy_t = Policy;
    Searcher<policy_t> s{};
};

TYPED_TEST_SUITE(UpperBoundTests, PoliciesUnderTest);

static std::vector<int> make_sorted_with_dupes(std::size_t n, int distinct = 50, std::uint32_t seed = 123) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, distinct - 1);

    std::vector<int> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) v.push_back(dist(rng));
    std::sort(v.begin(), v.end());
    return v;
}

static std::vector<Elem> make_sorted_elems(std::size_t n, int distinct = 50, std::uint32_t seed = 456) {
    auto keys = make_sorted_with_dupes(n, distinct, seed);
    std::vector<Elem> v;
    v.reserve(n);
    for (std::size_t i = 0; i < keys.size(); ++i) v.push_back(Elem{keys[i], static_cast<int>(i)});
    return v;
}

// ------------------------------------------------------------
// Core correctness: empty / single / boundaries
// ------------------------------------------------------------
TYPED_TEST(UpperBoundTests, EmptyRange_ReturnsFirst) {
    std::vector<int> v;
    auto it = this->s.upper_bound(v.begin(), v.end(), 10);
    EXPECT_EQ(it, v.begin());
}

TYPED_TEST(UpperBoundTests, SingleElement_AllCases) {
    std::vector<int> v{10};

    EXPECT_EQ(this->s.upper_bound(v.begin(), v.end(), 5), v.begin());
    EXPECT_EQ(this->s.upper_bound(v.begin(), v.end(), 10), v.end());
    EXPECT_EQ(this->s.upper_bound(v.begin(), v.end(), 11), v.end());
}

TYPED_TEST(UpperBoundTests, Duplicates_ReturnsPastLastOccurrence) {
    std::vector<int> v{1, 2, 2, 2, 3, 4};
    auto it = this->s.upper_bound(v.begin(), v.end(), 2);
    ASSERT_TRUE(it == v.begin() + 4);
    EXPECT_EQ(std::distance(v.begin(), it), 4);
}

TYPED_TEST(UpperBoundTests, AllLess_AndAllGreater) {
    std::vector<int> v{10, 20, 30};

    EXPECT_EQ(this->s.upper_bound(v.begin(), v.end(), -100), v.begin());
    EXPECT_EQ(this->s.upper_bound(v.begin(), v.end(), 1000), v.end());
}

// ------------------------------------------------------------
// Compare against std::upper_bound (int)
// ------------------------------------------------------------
TYPED_TEST(UpperBoundTests, MatchesStdUpperBound_Int_Vector) {
    auto v = make_sorted_with_dupes(10'000, 200, 1);

    for (int q : {-1, 0, 1, 50, 199, 200, 500}) {
        auto it1 = this->s.upper_bound(v.begin(), v.end(), q);
        auto it2 = std::upper_bound(v.begin(), v.end(), q);
        EXPECT_EQ(std::distance(v.begin(), it1), std::distance(v.begin(), it2));
    }
}

// ------------------------------------------------------------
// Pointer + span overloads (int)
// ------------------------------------------------------------
TYPED_TEST(UpperBoundTests, PointerOverload_MatchesStd) {
    std::array<int, 8> a{1, 2, 2, 5, 9, 10, 10, 11};

    int* first = a.data();
    int* last  = a.data() + a.size();

    for (int q : {0, 1, 2, 3, 10, 12}) {
        auto it1 = this->s.upper_bound(first, last, q);
        auto it2 = std::upper_bound(first, last, q);
        EXPECT_EQ(it1, it2);
    }
}

TYPED_TEST(UpperBoundTests, SpanOverload_MatchesStd) {
    std::array<int, 8> a{1, 2, 2, 5, 9, 10, 10, 11};

    std::span<int> s(a);
    std::span<const int> cs(a);

    for (int q : {0, 1, 2, 3, 10, 12}) {
        auto it1 = this->s.upper_bound(s, q);
        auto it2 = std::upper_bound(a.begin(), a.end(), q);
        EXPECT_EQ(it1, a.data() + std::distance(a.begin(), it2));

        auto it3 = this->s.upper_bound(cs, q);
        EXPECT_EQ(it3, a.data() + std::distance(a.begin(), it2));
    }
}

// ------------------------------------------------------------
// Heterogeneous search tests (Elem, key)
// ------------------------------------------------------------
TYPED_TEST(UpperBoundTests, Heterogeneous_OneWayComparator_Works) {
    auto v = make_sorted_elems(10'000, 200, 2);

    for (int q : {-1, 0, 1, 50, 199, 200, 500}) {
        auto it1 = this->s.upper_bound(v.begin(), v.end(), q, one_way_upper_comp);

        ElemKeyStrictLess strict{};
        auto it2 = std::upper_bound(v.begin(), v.end(), q, strict);

        EXPECT_EQ(std::distance(v.begin(), it1), std::distance(v.begin(), it2));
    }
}

TYPED_TEST(UpperBoundTests, Heterogeneous_StrictComparator_Overload_MatchesStd) {
    auto v = make_sorted_elems(50'000, 500, 3);

    ElemKeyStrictLess strict{};

    for (int q : {-10, 0, 5, 123, 499, 500, 999}) {
        auto it1 = this->s.upper_bound_strict(v.begin(), v.end(), q, strict);
        auto it2 = std::upper_bound(v.begin(), v.end(), q, strict);
        EXPECT_EQ(std::distance(v.begin(), it1), std::distance(v.begin(), it2));
    }
}

// ------------------------------------------------------------
// Property-style randomized test (int)
// ------------------------------------------------------------
TYPED_TEST(UpperBoundTests, Randomized_PropertyTest_AgreesWithStd) {
    std::mt19937 rng(999);
    std::uniform_int_distribution<int> qdist(-50, 250);

    auto v = make_sorted_with_dupes(100'000, 200, 4);

    for (int i = 0; i < 2000; ++i) {
        int q = qdist(rng);

        auto it1 = this->s.upper_bound(v.begin(), v.end(), q);
        auto it2 = std::upper_bound(v.begin(), v.end(), q);

        ASSERT_TRUE(it1 >= v.begin() && it1 <= v.end());
        EXPECT_EQ(std::distance(v.begin(), it1), std::distance(v.begin(), it2));

        // Invariant: all before are <= q; at it is > q (if not end)
        for (auto it = v.begin(); it != it1; ++it) {
            ASSERT_LE(*it, q);
        }
        if (it1 != v.end()) {
            ASSERT_GT(*it1, q);
        }
    }
}

} // namespace
