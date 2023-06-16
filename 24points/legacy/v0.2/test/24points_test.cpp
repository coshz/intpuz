#include "../src/24points.h"
#include "gtest/gtest.h"


TEST(test_obtain_idxs, obtain_idxs)
{
    // power, binominal
    ASSERT_EQ(Model<4>::obtain_idxs(3,6,false).size(), 120);
    ASSERT_EQ(Model<4>::obtain_idxs(3,6,true).size(), 216);
}

TEST(test_find_cts, find_cts)
{
    //catalan number
    ASSERT_EQ(Model<4>::cts_size(), 5);
    ASSERT_EQ(Model<8>::cts_size(), 429);
}
