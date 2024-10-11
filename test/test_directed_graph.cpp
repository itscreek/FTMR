#include <unordered_set>

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

TEST(DirectedGraphTest, StronglyConnectedComponents) {
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 3}, {4, 5}, {5, 4}};
    DirectedGraph digraph(edges, 6);
    std::vector<std::vector<int>> expected_vector = {{0, 1, 2}, {3, 4, 5}};
    std::vector<std::vector<int>> result =
        digraph.StronglyConnectedComponents();
    ASSERT_EQ(expected_vector.size(), result.size());
}

TEST(DirectedGraphTest, SimpleCycles) {
    std::vector<std::pair<int, int>> edges1 = {{0, 2}, {1, 0}, {2, 3}, {2, 4},
                                               {3, 1}, {3, 4}, {4, 5}, {5, 3}};
    DirectedGraph digraph1(edges1, 6);
    std::vector<std::vector<int>> expected_vector1 = {
        {0, 2, 3, 1, 0}, {0, 2, 4, 5, 3, 1, 0}, {3, 4, 5, 3}};
    std::vector<std::vector<int>> result1 = digraph1.SimpleCycles();
    ASSERT_EQ(expected_vector1, result1);

    std::vector<std::pair<int, int>> edges2 = {{0, 1}, {1, 2}, {2, 3}, {3, 1}};
    DirectedGraph digraph2(edges2, 4);
    std::vector<std::vector<int>> expected_vector2 = {{1, 2, 3, 1}};
    std::vector<std::vector<int>> result2 = digraph2.SimpleCycles();
    ASSERT_EQ(expected_vector2, result2);
}

TEST(DirectedGraphTest, DeleteCyclesOfLength2) {
    std::vector<std::pair<int, int>> edges = {
        {0, 2}, {1, 0}, {2, 0}, {2, 3}, {2, 4}, {3, 1}, {3, 5}, {4, 5}, {5, 4}};
    DirectedGraph digraph(edges, 6);
    DirectedGraph result = digraph.DeleteCyclesOfLength2();

    std::vector<std::pair<int, int>> expected_edges = {
        {1, 0}, {2, 3}, {2, 4}, {3, 1}, {3, 5}};
    ASSERT_EQ(expected_edges, result.Edges());
}
}  // namespace FTMR