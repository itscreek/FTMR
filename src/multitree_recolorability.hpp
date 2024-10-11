#pragma once

#include "directed_graph.hpp"

namespace FTMR {

class MultitreeRecolorability {
   public:
    MultitreeRecolorability() = default;

    ~MultitreeRecolorability() = default;

    MultitreeRecolorability(const DirectedGraph& digraph);

    MultitreeRecolorability(const std::vector<std::pair<int, int>>& edges,
                            int num_vertices);

    std::pair<int, int> GetPath(int path_number) {
        return path_relation_graph_vertices_[path_number];
    }

    int GetPathNumber(std::pair<int, int> path);

    // Returns true if and only if vertex_end is reachable from vertex_start.
    bool IsReachable(int vertex_start, int vertex_end);

    // Returns true if and only if the multitree satisfies the condition S.
    bool CheckConditionS();

    // Returns true if and only if the multitree satisfies the condition Cycle.
    bool CheckConditionCycle();

    bool CheckConditionCP();

    bool CheckConditionCV();

   private:
    DirectedGraph multitree_;

    std::vector<std::vector<int>> unilaterally_connected_components_;

    DirectedGraph path_relation_graph_;

    std::vector<std::pair<int, int>> path_relation_graph_vertices_;

    void ConstructPathRelationGraph();

    int GetNextStepPathNumber(int path_number);

    bool CheckConditionCPOnPath(int path_number,
                                const std::vector<int>& component);

    bool CheckConditionCVOnPathCycle(std::vector<int> path_cycle);
};

}  // namespace FTMR