#include <gtest/gtest.h>
#include "../include/hybridSearch/searcher.hpp"


TEST(Smoke, GTestWiresUp) {
    EXPECT_EQ(1 + 1, 2);
    auto s = hybrid_search::searcher<hybrid_search::policy::standard_binary>();
}


