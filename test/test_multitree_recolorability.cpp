#include "gtest/gtest.h"
#include "multitree_recolorability.hpp"

namespace FTMR {

TEST(MultitreeRecolorabilityTest, ConditionS) {
    const std::vector<std::pair<int, int>> edges1 = {
        {0, 1}, {0, 2}, {2, 4}, {3, 2}, {4, 5}, {4, 6}, {7, 6}};
    MultitreeRecolorability satisfyingS(edges1, 8);
    ASSERT_TRUE(satisfyingS.CheckConditionS());

    const std::vector<std::pair<int, int>> edges2 = {
        {0, 1}, {1, 2}, {1, 3}, {4, 3}, {3, 5}};
    MultitreeRecolorability not_satisfyingS(edges2, 6);
    ASSERT_FALSE(not_satisfyingS.CheckConditionS());
}

TEST(MultitreeRecolorabilityTest, ConditionCycle) {
    const std::vector<std::pair<int, int>> edges1 = {
        {0, 1}, {1, 2}, {3, 1}, {4, 2}, {0, 5},
        {5, 7}, {5, 6}, {8, 6}, {9, 4}, {9, 8}};
    MultitreeRecolorability satisfyingCycle(edges1, 10);
    ASSERT_TRUE(satisfyingCycle.CheckConditionCycle());

    const std::vector<std::pair<int, int>> edges2 = {
        {0, 1}, {1, 2}, {2, 4}, {3, 4}, {4, 5}, {5, 6}, {5, 7}, {7, 8}, {8, 9}};
    MultitreeRecolorability not_satisfyingCycle(edges2, 10);
    ASSERT_FALSE(not_satisfyingCycle.CheckConditionCycle());
}

TEST(MultitreeRecolorabilityTest, ConditionCP) {
    const std::vector<std::pair<int, int>> edges1 = {
        {0, 1}, {0, 2}, {2, 4}, {3, 2}, {4, 5}, {4, 6}, {7, 6}};
    MultitreeRecolorability satisfyingCP(edges1, 8);
    ASSERT_TRUE(satisfyingCP.CheckConditionCP());

    const std::vector<std::pair<int, int>> edges2 = {
        {0, 1}, {1, 2}, {2, 8}, {8, 3}, {3, 4}, {4, 5}, {2, 6}, {7, 3}};
    MultitreeRecolorability not_satisfyingCP(edges2, 9);
    ASSERT_FALSE(not_satisfyingCP.CheckConditionCP());
}

TEST(MultitreeRecolorabilityTest, ConditionCV) {
    const std::vector<std::pair<int, int>> edges1 = {
        {0, 1}, {1, 2}, {3, 1}, {4, 2}, {0, 5},
        {5, 7}, {5, 6}, {8, 6}, {9, 4}, {9, 8}};
    MultitreeRecolorability satisfyingCV(edges1, 10);
    ASSERT_TRUE(satisfyingCV.CheckConditionCV());
}
}  // namespace FTMR