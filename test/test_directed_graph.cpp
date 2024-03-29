#include "directed_graph.hpp"
#include "gtest/gtest.h"

namespace FTMR {
TEST(DirectedGraphTest, EdgesList) {
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 3}, {2, 3}, {3, 4}};
    DirectedGraph digraph(edges, 5);
    ASSERT_EQ(edges, digraph.Edges());
}

TEST(DirectedGraphTest, IsDAG) {
    const std::vector<std::pair<int, int>> edges1 = {{0, 1}, {0, 2}, {1, 3},
                                                     {1, 4}, {2, 5}, {2, 6}};
    DirectedGraph dag(edges1, 7);
    ASSERT_TRUE(dag.IsDAG());

    const std::vector<std::pair<int, int>> edges2 = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 3}};
    DirectedGraph not_dag(edges2, 5);
    ASSERT_FALSE(not_dag.IsDAG());
}

TEST(DirectedGraphTest, UnilaterallyConnectedComponents) {
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {1, 4}, {2, 5}, {2, 6},
        {7, 5}, {7, 8}, {9, 5}, {9, 8}, {10, 9}};

    DirectedGraph digraph(edges, 11);
    std::vector<std::vector<int>> expected_vector = {
        {0, 1, 4}, {0, 2, 5}, {0, 2, 6},  {0, 3},
        {7, 5},    {7, 8},    {10, 9, 5}, {10, 9, 8}};
    std::vector<std::vector<int>> result =
        digraph.UnilaterallyConnectedComponents();
    ASSERT_EQ(expected_vector, result);
}
}  // namespace FTMR