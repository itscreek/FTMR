#include "multitree_recolorability.hpp"

#include <algorithm>

namespace FTMR {
MultitreeRecolorability::MultitreeRecolorability(const DirectedGraph &digraph)
    : multitree_(digraph), path_relation_graph_vertices_() {
    unilaterally_connected_components_ =
        multitree_.UnilaterallyConnectedComponents();
    ConstructPathRelationGraph();
}

MultitreeRecolorability::MultitreeRecolorability(
    const std::vector<std::pair<int, int>> &edges, int num_vertices)
    : multitree_(edges, num_vertices), path_relation_graph_vertices_() {
    unilaterally_connected_components_ =
        multitree_.UnilaterallyConnectedComponents();
    ConstructPathRelationGraph();
}

int MultitreeRecolorability::GetPathNumber(std::pair<int, int> path) {
    auto itr = std::find(path_relation_graph_vertices_.begin(),
                         path_relation_graph_vertices_.end(), path);
    if (itr != path_relation_graph_vertices_.end()) {
        return std::distance(path_relation_graph_vertices_.begin(), itr);
    }
    return -1;
}

bool MultitreeRecolorability::IsReachable(int vertex_start, int vertex_end) {
    if (unilaterally_connected_components_.size() == 0) {
        unilaterally_connected_components_ =
            multitree_.UnilaterallyConnectedComponents();
    }

    for (const auto &component : unilaterally_connected_components_) {
        auto itr1 = std::find(component.begin(), component.end(), vertex_start);
        auto itr2 = std::find(component.begin(), component.end(), vertex_end);
        if (itr1 != component.end() && itr2 != component.end() && itr1 < itr2) {
            return true;
        }
    }
    return false;
}

bool MultitreeRecolorability::CheckConditionS() {
    for (const auto &component : unilaterally_connected_components_) {
        std::vector<int> merging_vertex_order;
        std::vector<int> splitting_vertex_order;
        int number = 0;
        for (const auto &vertex : component) {
            if (multitree_.InDegree(vertex) >= 2) {
                merging_vertex_order.push_back(number);
            }

            if (multitree_.OutDegree(vertex) >= 2) {
                splitting_vertex_order.push_back(number);
            }
            ++number;
        }

        for (const auto &m_order_number : merging_vertex_order) {
            for (const auto &s_order_number : splitting_vertex_order) {
                if (m_order_number > s_order_number &&
                    m_order_number != component.size() - 1 &&
                    s_order_number != 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool MultitreeRecolorability::CheckConditionCycle() {
    const std::vector<std::pair<int, int>> edges_of_prg =
        path_relation_graph_.Edges();

    std::vector<std::pair<int, int>> edges;
    for (auto &edge : edges_of_prg) {
        if (std::find(edges_of_prg.begin(), edges_of_prg.end(),
                      std::pair<int, int>(edge.second, edge.first)) ==
            edges_of_prg.end()) {
            edges.push_back(edge);
        }
    }

    DirectedGraph digraph(edges, path_relation_graph_.NumVertices());
    return digraph.IsDAG();
}

int MultitreeRecolorability::GetNextStepPathNumber(int path_number) {
    std::pair<int, int> path = GetPath(path_number);

    if (path.first == path.second) {
        return path_number;
    }

    for (auto &adjacent_vertex : multitree_.AdjacentVertices(path.first)) {
        auto itr = std::find(path_relation_graph_vertices_.begin(),
                             path_relation_graph_vertices_.end(),
                             std::pair<int, int>(adjacent_vertex, path.second));
        if (itr != path_relation_graph_vertices_.end()) {
            return std::distance(path_relation_graph_vertices_.begin(), itr);
        }
    }
    return path_number;
}

bool MultitreeRecolorability::CheckConditionCP() {
    DirectedGraph path_relation_without_cycles =
        path_relation_graph_.DeleteCyclesOfLength2();
    std::vector<std::vector<int>> strongly_connected_components =
        path_relation_without_cycles.StronglyConnectedComponents();

    for (const auto &component : strongly_connected_components) {
        for (auto &path_number : component) {
            bool condition_cp_on_path =
                CheckConditionCPOnPath(path_number, component);
            if (!condition_cp_on_path) {
                return false;
            }
        }
    }

    return true;
}

bool MultitreeRecolorability::CheckConditionCPOnPath(
    int path_number, const std::vector<int> &component) {
    int next_step_path_number = GetNextStepPathNumber(path_number);

    for (auto &adjacent_path_number :
         path_relation_graph_.AdjacentVertices(path_number)) {
        if (std::find(component.begin(), component.end(),
                      adjacent_path_number) == component.end()) {
            continue;
        }

        for (auto &radjacent_path_number :
             path_relation_graph_.ReverseAdjacentVertices(path_number)) {
            if (std::find(component.begin(), component.end(),
                          radjacent_path_number) == component.end()) {
                continue;
            }

            bool cycle2_appear = path_relation_graph_.IsAdjacent(
                adjacent_path_number, next_step_path_number);

            bool cycle_persist =
                IsReachable(GetPath(next_step_path_number).first,
                            GetPath(radjacent_path_number).second);

            if (!cycle2_appear && !cycle_persist) {
                return false;
            }
        }
    }

    return true;
}

bool MultitreeRecolorability::CheckConditionCV() {
    DirectedGraph path_relation_without_cycles =
        path_relation_graph_.DeleteCyclesOfLength2();
    std::vector<std::vector<int>> simple_path_cycles =
        path_relation_without_cycles.SimpleCycles();

    for (auto &path_cycle : simple_path_cycles) {
        bool condition_cv_on_path_cycle =
            CheckConditionCVOnPathCycle(path_cycle);
        if (!condition_cv_on_path_cycle) {
            return false;
        }
    }
    return true;
}

bool MultitreeRecolorability::CheckConditionCVOnPathCycle(
    std::vector<int> path_cycle) {
    // path_cycle starts and ends with the same vertex
    // example of a path_cycle: 0 -> 1 -> 2 -> 3 -> 0
    for (int i = 0; i < path_cycle.size() - 1; ++i) {
        int path_number = path_cycle[i];
        int adjacent_path_number = path_cycle[i + 1];
        int next_step_path_number = GetNextStepPathNumber(path_number);

        if (!path_relation_graph_.IsAdjacent(adjacent_path_number,
                                             next_step_path_number)) {
            return true;
        }
    }
    return false;
}

void MultitreeRecolorability::ConstructPathRelationGraph() {
    std::vector<std::vector<int>> path_list;
    for (auto &component : unilaterally_connected_components_) {
        for (int i = 0; i < component.size(); ++i) {
            for (int j = i; j < component.size(); ++j) {
                std::pair<int, int> path(component[i], component[j]);
                if (std::find(path_relation_graph_vertices_.begin(),
                              path_relation_graph_vertices_.end(),
                              path) == path_relation_graph_vertices_.end()) {
                    path_relation_graph_vertices_.push_back(path);
                    path_list.push_back(std::vector<int>(
                        component.begin() + i, component.begin() + j + 1));
                }
            }
        }
    }

    std::vector<std::pair<int, int>> edges;
    int vertex1_index = 0;
    for (const auto &prg_vertex1 : path_relation_graph_vertices_) {
        int vertex2_index = 0;
        for (const auto &prg_vertex2 : path_relation_graph_vertices_) {
            if (prg_vertex1 == prg_vertex2) {
                ++vertex2_index;
                continue;
            }

            bool prg_v2_first_in_prg_v1 =
                std::find(path_list[vertex1_index].begin(),
                          path_list[vertex1_index].end(),
                          prg_vertex2.first) != path_list[vertex1_index].end();
            bool prg_v1_second_in_prg_v2 =
                std::find(path_list[vertex2_index].begin(),
                          path_list[vertex2_index].end(),
                          prg_vertex1.second) != path_list[vertex2_index].end();

            if (prg_v2_first_in_prg_v1 || prg_v1_second_in_prg_v2) {
                edges.push_back({vertex1_index, vertex2_index});
            }

            ++vertex2_index;
        }

        ++vertex1_index;
    }

    path_relation_graph_ =
        DirectedGraph(edges, path_relation_graph_vertices_.size());
}
}  // namespace FTMR