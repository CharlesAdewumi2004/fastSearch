#pragma once;

#include <cstddef>
#include <span>
#include <functional>

#include "details/lower-bound-gallop-hybrid-impl.hpp"
#include "details/lower-bound-gallop-impl.hpp"
#include "details/lower-bound-hybrid-impl.hpp"
#include "details/lower-bound-standard-impl.hpp"

template <class Policy>
class searcher final
{
public:
    searcher() = default ~searcher() = default;

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
