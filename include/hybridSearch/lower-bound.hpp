#pragma once

#include "policy.hpp"
#include "details/lower-bound-impl.hpp"
#include <functional>

namespace hybridSearch
{

    template <class It, class V>
    It lower_bound(It first, It last, const V &value)
    {
        return lower_bound(policy::standard_binary{}, first, last, value, std::less<>{});
    }

    template <class Policy, class It, class V>
    It lower_bound(Policy pol, It first, It last, const V &value)
    {
        return lower_bound(pol, first, last, value, std::less<>{});
    }

    template <class It, class V, class Comp>
    It lower_bound(It first, It last, const V &value, Comp comp)
    {
        return lower_bound(policy::standard_binary{}, first, last, value, comp);
    }

    template <class Policy, class It, class V, class Comp>
    It lower_bound(Policy pol, It first, It last, const V &value, Comp comp)
    {
        return detail::lower_bound_impl(pol, first, last, value, comp);
    }
    template <class T, class V>
    T *lower_bound(std::span<T> s, const V &value)
    {
        return lower_bound(policy::standard_binary{}, s, value, std::less<>{});
    }

    template <class Policy, class T, class V>
    T *lower_bound(Policy pol, std::span<T> s, const V &value)
    {
        return lower_bound(pol, s, value, std::less<>{});
    }

    template <class T, class V, class Comp>
    T *lower_bound(std::span<T> s, const V &value, Comp comp)
    {
        return lower_bound(policy::standard_binary{}, s, value, comp);
    }

    template <class Policy, class T, class V, class Comp>
    T *lower_bound(Policy pol, std::span<T> s, const V &value, Comp comp)
    {
        return detail::lower_bound_impl(pol, s.data(), s.data() + s.size(), value, comp);
    }
    template <class T, class V>
    T *lower_bound(T *first, T *last, const V &value)
    {
        return lower_bound(policy::standard_binary{}, first, last, value, std::less<>{});
    }

    template <class Policy, class T, class V>
    T *lower_bound(Policy pol, T *first, T *last, const V &value)
    {
        return lower_bound(pol, first, last, value, std::less<>{});
    }

    template <class T, class V, class Comp>
    T *lower_bound(T *first, T *last, const V &value, Comp comp)
    {
        return lower_bound(policy::standard_binary{}, first, last, value, comp);
    }

    template <class Policy, class T, class V, class Comp>
    T *lower_bound(Policy pol, T *first, T *last, const V &value, Comp comp)
    {
        return detail::lower_bound_impl(pol, first, last, value, comp);
    };

}