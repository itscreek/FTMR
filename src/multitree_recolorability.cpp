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

void MultitreeRecolorability::ConstructPathRelationGraph() {
    std::vector<std::vector<int>> path_list;
    for (auto &component : unilaterally_connected_components_) {
        for (int i = 0; i < component.size(); ++i) {
            for (int j = i + 1; j < component.size(); ++j) {
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

            if (prg_vertex1.first != prg_vertex2.first &&
                prg_vertex1.second != prg_vertex2.second &&
                (prg_v2_first_in_prg_v1 || prg_v1_second_in_prg_v2)) {
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