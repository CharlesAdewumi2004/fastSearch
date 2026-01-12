#pragma once

#include <cstddef>
#include <span>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

#include "details/lower-bound-gallop-impl.hpp"
#include "details/lower-bound-hybrid-impl.hpp"
#include "details/lower-bound-standard-impl.hpp"
#include "policy.hpp"
#include "traits.hpp"

namespace boundcraft
{
    // ------------------------------------------------------------
    // Comparator concepts
    // ------------------------------------------------------------

    // 1-way heterogeneous "key-compare": comp(element, key) -> bool
    template <class Comp, class It, class V>
    concept lower_bound_key_compare =
        std::input_iterator<It> &&
        requires(Comp comp, std::iter_reference_t<It> elem, const V &v)
        {
            { std::invoke(comp, elem, v) } -> std::convertible_to<bool>;
        };

    // strict weak order (std::lower_bound-style comparator)
    template <class Comp, class It, class V>
    concept lower_bound_strict_compare =
        std::input_iterator<It> &&
        std::indirect_strict_weak_order<Comp, It, const V *>;

    template <class Policy>
    class searcher final
    {
    public:
        searcher() = default;
        ~searcher() = default;

        // ------------------------------------------------------------
        // Iterators
        // ------------------------------------------------------------

        template <class It, class V>
        It lower_bound(It first, It last, const V &value)
        {
            // default comparator expects (*it < value) to be valid
            return lower_bound(first, last, value, std::less<>{});
        }

        // ✅ FIXED: accept 1-way comparator (elem, key)
        template <class It, class V, class Comp>
            requires lower_bound_key_compare<Comp, It, V>
        It lower_bound(It first, It last, const V &value, Comp comp)
        {
            return dispatch(first, last, value, comp);
        }

        // Optional: keep a strict-order entry point for people who want it
        template <class It, class V, class Comp>
            requires lower_bound_strict_compare<Comp, It, V>
        It lower_bound_strict(It first, It last, const V &value, Comp comp)
        {
            return dispatch(first, last, value, comp);
        }

        // ------------------------------------------------------------
        // std::span
        // ------------------------------------------------------------

        template <class T, class V>
        T *lower_bound(std::span<T> s, const V &value)
        {
            return lower_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires lower_bound_key_compare<Comp, T *, V>
        T *lower_bound(std::span<T> s, const V &value, Comp comp)
        {
            return dispatch(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        const T *lower_bound(std::span<const T> s, const V &value)
        {
            return lower_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires lower_bound_key_compare<Comp, const T *, V>
        const T *lower_bound(std::span<const T> s, const V &value, Comp comp)
        {
            return dispatch(s.data(), s.data() + s.size(), value, comp);
        }

        // ------------------------------------------------------------
        // Pointers
        // ------------------------------------------------------------

        template <class T, class V>
        T *lower_bound(T *first, T *last, const V &value)
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires lower_bound_key_compare<Comp, T *, V>
        T *lower_bound(T *first, T *last, const V &value, Comp comp)
        {
            return dispatch(first, last, value, comp);
        }

        template <class T, class V>
        const T *lower_bound(const T *first, const T *last, const V &value)
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires lower_bound_key_compare<Comp, const T *, V>
        const T *lower_bound(const T *first, const T *last, const V &value, Comp comp)
        {
            return dispatch(first, last, value, comp);
        }

    private:
        // single policy switch so gallop changes apply everywhere
        template <class It, class V, class Comp>
            requires lower_bound_key_compare<Comp, It, V>
        static It dispatch(It first, It last, const V &value, Comp comp);
    };

    // =========================================================================
    // Dispatch (single policy switch)
    // =========================================================================

    template <class Policy>
    template <class It, class V, class Comp>
        requires lower_bound_key_compare<Comp, It, V>
    It searcher<Policy>::dispatch(It first, It last, const V &value, Comp comp)
    {
        using traits = boundcraft::policy::traits::policy_traits<Policy>;
        constexpr auto k = traits::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return boundcraft::detail::lower_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            constexpr std::size_t threshold = traits::threshold;
            return boundcraft::detail::lower_bound_hybrid_impl(threshold, first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            using search_policy_t = typename traits::search_policy;
            using gallop_start_t  = typename traits::gallop_start;

            return boundcraft::detail::lower_bound_gallop_impl<
                search_policy_t,
                gallop_start_t>(first, last, value, comp);
        }
        else
        {
            static_assert([] { return false; }(), "Unknown policy");
        }
    }

} 
