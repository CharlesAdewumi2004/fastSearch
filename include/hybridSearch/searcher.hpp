#pragma once

#include <cstddef>
#include <span>
#include <functional>

#include "details/lower-bound-gallop-hybrid-impl.hpp"
#include "details/lower-bound-gallop-impl.hpp"
#include "details/lower-bound-hybrid-impl.hpp"
#include "details/lower-bound-standard-impl.hpp"
#include "policy.hpp"
#include "traits.hpp"

namespace hybrid_search
{
    template <class Policy>
    class searcher final
    {
    public:
        searcher() = default;
        ~searcher() = default;

        template <class It, class V>
        It lower_bound(It first, It last, const V &value);

        template <class It, class V, class Comp>
        It lower_bound(It first, It last, const V &value, Comp comp);

        template <class T, class V>
        T *lower_bound(std::span<T> s, const V &value);

        template <class T, class V, class Comp>
        T *lower_bound(std::span<T> s, const V &value, Comp comp);

        template <class T, class V>
        T *lower_bound(T *first, T *last, const V &value);

        template <class T, class V, class Comp>
        T *lower_bound(T *first, T *last, const V &value, Comp comp);

    private:
        using search_policy = Policy;
        std::size_t last_idx = 0;
        bool has_hint = false;
    };

    template <class Policy>
    template <class It, class V>
    It searcher<Policy>::lower_bound(It first, It last, const V &value)
    {
        constexpr auto k = hybrid_search::policy::traits::policy_traits<Policy>::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            // TODO
        }
        else if constexpr (k == policy_kind::galloping)
        {
            // TODO
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            // TODO
        }
        else
        {
            static_assert([]{ return false; }(), "Unknown policy");
        }
    }

}