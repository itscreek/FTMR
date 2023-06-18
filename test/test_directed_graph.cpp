#include "gtest/gtest.h"
#include "directed_graph.hpp"

namespace FTMR {
TEST(DirectedGraphTest, UnilaterallyConnectedComponents){
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {1, 4}, {2, 5}, {2, 6},
        {7, 5}, {7, 8}, {9, 5}, {9, 8}, {10, 9}};
    
    DirectedGraph digraph(edges, 11);
    std::vector<std::vector<int>> expected_vector = {
        {0, 1, 4}, {0, 2, 5}, {0, 2, 6},  {0, 3},
        {7, 5},    {7, 8},    {10, 9, 5}, {10, 9, 8}};
    std::vector<std::vector<int>> result = digraph.UnilaterallyConnectedComponents();
    ASSERT_EQ(expected_vector, result);
}
}