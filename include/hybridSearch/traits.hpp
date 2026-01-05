#pragma once

#include "policy.hpp"

enum class policy_kind
{
    standard_binary,
    galloping,
    hybrid
};

namespace hybrid_search::policy::traits
{

    template <class Policy>
    struct policy_traits;

    template <>
    struct policy_traits<standard_binary>
    {
        static constexpr policy_kind kind = policy_kind::standard_binary;
    };

    template <class Search_Policy, class Gallop_Start>
    struct policy_traits<galloping<Search_Policy, Gallop_Start>>
    {
        static constexpr policy_kind kind = policy_kind::galloping;
        using policy_search_policy = Search_Policy;
        using gallop_start = Gallop_Start; 
    };

    template <std::size_t T>
    struct policy_traits<hybrid<T>>
    {
        static constexpr policy_kind kind = policy_kind::hybrid;
        static constexpr std::size_t threshold = T;
    };
};
