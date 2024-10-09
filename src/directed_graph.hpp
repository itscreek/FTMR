#pragma once

#include <utility>
#include <vector>

namespace FTMR {
class DirectedGraph {
   public:
    DirectedGraph() = default;

    ~DirectedGraph() = default;

    DirectedGraph(const std::vector<std::pair<int, int>>& edges_list,
                  int num_vertices);

    int NumVertices() { return num_vertices_; }

    int NumEdges() { return num_edges_; }

    // Returns the outdegree of the vertex
    int OutDegree(int vertex);

    // Returns the indegree of the vertex
    int InDegree(int vertex);

    // Returns a vector of the edges of the graph
    std::vector<std::pair<int, int>> Edges();

    // Returns true when the graph is DAG
    bool IsDAG();

    // Returns unilaterally connected components of the graph.
    // This function only work when the graph is DAG
    std::vector<std::vector<int>> UnilaterallyConnectedComponents();

    std::vector<std::vector<int>> StronglyConnectedComponents();

    DirectedGraph DeleteCyclesOfLength2();

   private:
    int num_vertices_;
    int num_edges_;
    std::vector<std::vector<int>> adjacency_list_;
    std::vector<std::vector<int>> reverse_adjacency_list_;

    bool InvalidVertexNumber(int vertex) {
        return vertex < 0 || vertex >= num_vertices_;
    }

    // Adds vertex to connected_components_list[component_index] and performs
    // DFS.
    void PathSearch(int vertex,
                    std::vector<std::vector<int>>& connected_components_list,
                    int component_index);
};
}  // namespace FTMR