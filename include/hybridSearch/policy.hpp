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

    struct galloping final
    {
    };

    template <std::size_t Threshold>
    struct galloping_binary_linear_hybrid final
    {
    };

}