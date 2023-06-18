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

    int OutDegree(int vertex);

    int InDegree(int vertex);

    std::vector<std::vector<int>>
    UnilaterallyConnectedComponents();  // This function work well only if graph
                                        // is DAG

   private:
    int num_vertices_;
    int num_edges_;
    std::vector<std::vector<int>> adjacency_list_;
    std::vector<std::vector<int>> reverse_adjacency_list_;

    bool InvalidVertexNumber(int vertex) {
        return vertex < 0 || vertex >= num_vertices_;
    }

    void PathSearch(int vertex,
                    std::vector<std::vector<int>>& connected_components_list,
                    int component_index);
};
}  // namespace FTMR