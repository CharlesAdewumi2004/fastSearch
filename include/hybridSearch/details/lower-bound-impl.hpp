#pragma once

#include <cstddef>
#include <iterator>

namespace hybrid_search::detail
{

    template <class Policy, class It, class V, class Comp>
    It lower_bound_impl(Policy pol, It first, It last, const V &value, Comp comp)
    {
    }

    template <class It, class V, class Comp>
    inline It lower_bound_linear_scan(It first, typename std::iterator_traits<It>::difference_type count, const V &value, Comp comp)
    {
        while (count > 0)
        {
            if (!comp(*first, value))
            {
                break;
            }
            ++first;
            --count;
        }
        return first;
    }

    /* ============================================================
       RANDOM ACCESS ITERATORS
       ============================================================ */

    template <class RandomIt, class V, class Comp>
    inline void lower_bound_probe_ra(RandomIt &first, typename std::iterator_traits<RandomIt>::difference_type &count, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t step = count / 2;
        RandomIt mid = first + step;

        if (comp(*mid, value))
        {
            first = mid + 1;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    /* ============================================================
       STANDARD BINARY SEARCH
       ============================================================ */

    template <class RandomIt, class V, class Comp>
    inline RandomIt lower_bound_standard_binary_ra(RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t count = last - first;
        while (count > 0)
        {
            lower_bound_probe_ra(first, count, value, comp);
        }
        return first;
    }

    /* ============================================================
       HYBRID SEARCH
       ============================================================ */

    template <class RandomIt, class V, class Comp>
    inline RandomIt lower_bound_hybrid_ra(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t count = last - first;
        while (count > range)
        {
            lower_bound_probe_ra(first, count, value, comp);
        }
        lower_bound_linear_scan(first, count, value, comp);
        return first;
    }

    /* ============================================================
       FORWARD ITERATORS
       ============================================================ */

    template <class ForwardIt, class V, class Comp>
    inline void lower_bound_probe_fw(ForwardIt &first, typename std::iterator_traits<ForwardIt>::difference_type &count, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<ForwardIt>::difference_type;

        diff_t step = count / 2;
        ForwardIt mid = first;
        std::advance(mid, step);

        if (comp(*mid, value))
        {
            ++mid;
            first = mid;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    /*  ============================================================
        STANDARD BINARY SEARCH
        ============================================================ */

    template <class ForwardIt, class V, class Comp>
    inline ForwardIt lower_bound_standard_binary_fw(ForwardIt first, ForwardIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<ForwardIt>::difference_type;

        diff_t count = std::distance(first, last);
        while (count > 0)
        {
            lower_bound_probe_fw(first, count, value, comp);
        }
        return first;
    }

    /* ============================================================
       HYBRID SEARCH
       ============================================================ */

    template <class RandomIt, class V, class Comp>
    inline RandomIt lower_bound_hybrid_fw(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<ForwardIt>::difference_type;

        diff_t count = std::distance(first, last);
        while (count > 0)
        {
            lower_bound_probe_fw(first, count, value, comp);
        }
        lower_bound_linear_scan(first, count, value, comp);
        return first;
    }

}
