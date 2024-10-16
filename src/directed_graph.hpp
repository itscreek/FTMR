#pragma once

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace FTMR {
class DirectedGraph {
   public:
    DirectedGraph() = default;

    ~DirectedGraph() = default;

    DirectedGraph(const std::vector<std::pair<int, int>>& edges_list,
                  int num_vertices);

    int NumVertices() const { return num_vertices_; }

    int NumEdges() const { return num_edges_; }

    // Returns the outdegree of the vertex
    int OutDegree(int vertex) const;

    // Returns the indegree of the vertex
    int InDegree(int vertex) const;

    // Returns a vector of the edges of the graph
    std::vector<std::pair<int, int>> Edges() const;

    std::vector<int> AdjacentVertices(int vertex) const {
        return adjacency_list_[vertex];
    }

    std::vector<int> ReverseAdjacentVertices(int vertex) const {
        return reverse_adjacency_list_[vertex];
    }

    bool IsAdjacent(int vertex1, int vertex2) const;

    // Returns true when the graph is DAG
    bool IsDAG() const;

    // Returns unilaterally connected components of the graph.
    // This function only work when the graph is DAG
    std::vector<std::vector<int>> UnilaterallyConnectedComponents() const;

    std::vector<std::vector<int>> StronglyConnectedComponents() const;

    std::vector<std::vector<int>> SimpleCycles() const;

    DirectedGraph CreateSubgraph(const std::vector<int>& vertices) const;

    DirectedGraph DeleteCyclesOfLength2() const;

   private:
    int num_vertices_;
    int num_edges_;
    std::vector<std::vector<int>> adjacency_list_;
    std::vector<std::vector<int>> reverse_adjacency_list_;

    bool InvalidVertexNumber(int vertex) const {
        return vertex < 0 || vertex >= num_vertices_;
    }

    // Adds vertex to connected_components_list[component_index] and performs
    // DFS.
    void PathSearch(int vertex,
                    std::vector<std::vector<int>>& connected_components_list,
                    int component_index) const;

    void DFSForSCC(std::vector<bool>& is_visited,
                   std::deque<int>& finished_vertices, int vertex) const;

    void ReverseDFSForSCC(std::vector<bool>& is_visited,
                          std::vector<int>& component, int vertex) const;

    void UnBlockJohnson(
        std::unordered_set<int>& blocked_set,
        std::unordered_map<int, std::unordered_set<int>>& blocked_map,
        int vertex) const;

    bool FindCyclesInSCCJohnson(
        const DirectedGraph& scc_graph, std::unordered_set<int>& blocked_set,
        std::unordered_map<int, std::unordered_set<int>>& blocked_map,
        std::deque<int>& stack, std::vector<std::vector<int>>& cycles,
        int start_vertex, int current_vertex) const;
};
}  // namespace FTMR