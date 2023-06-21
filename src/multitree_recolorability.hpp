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

    // Returns true if and only if the multitree satisfies the condition S.
    bool CheckConditionS();

    // Returns true if and only if the multitree satisfies the condition Cycle.
    bool CheckConditionCycle();

   private:
    DirectedGraph multitree_;

    std::vector<std::vector<int>> unilaterally_connected_components_;

    DirectedGraph path_relation_graph_;

    std::vector<std::pair<int, int>> path_relation_graph_vertices_;

    void ConstructPathRelationGraph();
};

}  // namespace FTMR