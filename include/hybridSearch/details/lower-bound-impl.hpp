#pragma once

#include <cstddef>
#include <iterator>

#include "lower-bound-gallop-impl.hpp"
#include "lower-bound-hybrid-impl.hpp"
#include "lower-bound-standard-impl.hpp"
#include "lower-bound-gallop-hybrid-impl.hpp"
namespace hybrid_search::detail
{

    template <class Policy, class It, class V, class Comp>
    It lower_bound_impl(Policy pol, It first, It last, const V &value, Comp comp)
    {
        
    }

}
