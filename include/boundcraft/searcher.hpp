#pragma once

#include <cstddef>
#include <span>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

#include <boundcraft/details/lower-bound/lower-bound.hpp>
#include <boundcraft/details/upper-bound/upper-bound.hpp>
#include <boundcraft/policy.hpp>
#include <boundcraft/traits.hpp>

namespace boundcraft
{
    // ------------------------------------------------------------
    // Comparator concepts
    // ------------------------------------------------------------

    // 1-way comparison
    template <class Comp, class It, class V>
    concept one_way_comparison =
        std::input_iterator<It> &&
        requires(Comp comp, std::iter_reference_t<It> elem, const V &v)
        {
            { std::invoke(comp, elem, v) } -> std::convertible_to<bool>;
        };
    
        // 2-way comparison
    template <class Comp, class It, class V>
    concept two_way_comparison =
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
            return lower_bound(first, last, value, std::less<>{});
        }


        template <class It, class V, class Comp>
            requires one_way_comparison<Comp, It, V>
        It lower_bound(It first, It last, const V &value, Comp comp)
        {
            return lower_bound_dispatch(first, last, value, comp);
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
            requires one_way_comparison<Comp, T *, V>
        T *lower_bound(std::span<T> s, const V &value, Comp comp)
        {
            return lower_bound_dispatch(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        const T *lower_bound(std::span<const T> s, const V &value)
        {
            return lower_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_comparison<Comp, const T *, V>
        const T *lower_bound(std::span<const T> s, const V &value, Comp comp)
        {
            return lower_bound_dispatch(s.data(), s.data() + s.size(), value, comp);
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
            requires one_way_comparison<Comp, T *, V>
        T *lower_bound(T *first, T *last, const V &value, Comp comp)
        {
            return lower_bound_dispatch(first, last, value, comp);
        }

        template <class T, class V>
        const T *lower_bound(const T *first, const T *last, const V &value)
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_comparison<Comp, const T *, V>
        const T *lower_bound(const T *first, const T *last, const V &value, Comp comp)
        {
            return lower_bound_dispatch(first, last, value, comp);
        }

    private:
        // single policy switch so gallop changes apply everywhere
        template <class It, class V, class Comp>
            requires one_way_comparison<Comp, It, V>
        static It lower_bound_dispatch(It first, It last, const V &value, Comp comp);
    };





    // =========================================================================
    // lower_bound_dispatch
    // =========================================================================

    template <class Policy>
    template <class It, class V, class Comp>
        requires one_way_comparison<Comp, It, V>
    It searcher<Policy>::lower_bound_dispatch(It first, It last, const V &value, Comp comp)
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
