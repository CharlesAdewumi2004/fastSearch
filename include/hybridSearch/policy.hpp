#pragma once;

#include <cstddef>

namespace hybridSearch::policy
{
    struct standard_binary final
    {
    };

    template <std::size_t Threshold>
    struct binary_linear_hybrid final
    {
        static constexpr std::size_t threshold = Threshold;
    };

    template <class Search_Policy, class Gallop_Start>
    struct galloping_binary_linear_hybrid final
    {
        using search_policy = Search_Policy;
        using gallop_start = Gallop_Start;
    };

}

namespace hybridSearch::policy::gallop
{
    struct start_front final
    {
    };
    struct start_back final
    {
    };
    struct start_middle final
    {
    };
    struct start_last_searched final
    {
    };
}