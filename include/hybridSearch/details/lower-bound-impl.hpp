#pragma once

#include <cstddef>
#include <iterator>

namespace hybrid_search::detail
{

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

}
