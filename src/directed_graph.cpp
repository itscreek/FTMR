#include "directed_graph.hpp"

#include <algorithm>
#include <deque>
#include <stdexcept>

namespace FTMR {
DirectedGraph::DirectedGraph(const std::vector<std::pair<int, int>>& edges_list,
                             int num_vertices)
    : num_vertices_(num_vertices),
      num_edges_(edges_list.size()),
      adjacency_list_(),
      reverse_adjacency_list_() {
    if (num_vertices <= 0) {
        throw std::invalid_argument(
            "Number of vertices must be greater than 0.");
    }

    adjacency_list_.resize(num_vertices_, std::vector<int>());
    reverse_adjacency_list_.resize(num_vertices_, std::vector<int>());

    for (int i = 0; i < edges_list.size(); i++) {
        int start_vertex = edges_list[i].first;
        int end_vertex = edges_list[i].second;

        if (InvalidVertexNumber(start_vertex) ||
            InvalidVertexNumber(end_vertex)) {
            throw std::invalid_argument(
                "Vertex number in edges list must be 0 to n - 1.");
        }

        adjacency_list_[start_vertex].push_back(end_vertex);
        reverse_adjacency_list_[end_vertex].push_back(start_vertex);
    }
}

int DirectedGraph::OutDegree(int vertex) const {
    if (InvalidVertexNumber(vertex)) {
        throw std::invalid_argument("Vertex number must be 0 to n - 1.");
    }

    return adjacency_list_[vertex].size();
}

int DirectedGraph::InDegree(int vertex) const {
    if (InvalidVertexNumber(vertex)) {
        throw std::invalid_argument("Vertex number must be 0 to n - 1.");
    }

    return reverse_adjacency_list_[vertex].size();
}

std::vector<std::pair<int, int>> DirectedGraph::Edges() const {
    std::vector<std::pair<int, int>> edges(num_edges_);

    int count = 0;
    int start_of_edge = 0;
    for (auto& adjacent_vertices : adjacency_list_) {
        for (auto& end_of_edge : adjacent_vertices) {
            edges[count] = {start_of_edge, end_of_edge};
            ++count;
        }
        ++start_of_edge;
    }

    return edges;
}

bool DirectedGraph::IsAdjacent(int vertex1, int vertex2) const {
    if (InvalidVertexNumber(vertex1) || InvalidVertexNumber(vertex2)) {
        throw std::invalid_argument("Vertex number must be 0 to n - 1.");
    }
    return std::find(adjacency_list_[vertex1].begin(),
                     adjacency_list_[vertex1].end(),
                     vertex2) != adjacency_list_[vertex1].end();
}

bool DirectedGraph::IsDAG() const {
    enum struct VertexState { NOT_VISITED, VISITING, VISITED };
    std::vector<VertexState> vertex_state(num_vertices_,
                                          VertexState::NOT_VISITED);

    for (int vertex = 0; vertex < num_vertices_; ++vertex) {
        if (vertex_state[vertex] == VertexState::VISITED) {
            continue;
        }

        std::deque<int> visiting_order_stack;
        visiting_order_stack.push_front(vertex);

        while (!visiting_order_stack.empty()) {
            int visiting_vertex = visiting_order_stack.front();

            if (vertex_state[visiting_vertex] == VertexState::VISITED) {
                visiting_order_stack.pop_front();
                continue;
            }

            if (vertex_state[visiting_vertex] == VertexState::VISITING) {
                visiting_order_stack.pop_front();
                vertex_state[visiting_vertex] = VertexState::VISITED;
                continue;
            }

            if (vertex_state[visiting_vertex] == VertexState::NOT_VISITED) {
                vertex_state[visiting_vertex] = VertexState::VISITING;

                for (auto& adjacent_vertex : adjacency_list_[visiting_vertex]) {
                    if (vertex_state[adjacent_vertex] ==
                        VertexState::NOT_VISITED) {
                        visiting_order_stack.push_front(adjacent_vertex);
                    }

                    if (vertex_state[adjacent_vertex] ==
                        VertexState::VISITING) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

std::vector<std::vector<int>> DirectedGraph::UnilaterallyConnectedComponents()
    const {
    std::vector<std::vector<int>> connected_components_list;

    for (int vertex = 0; vertex < num_vertices_; ++vertex) {
        if (reverse_adjacency_list_[vertex].size() == 0) {
            connected_components_list.push_back(std::vector<int>());
            int component_index = connected_components_list.size() - 1;
            PathSearch(vertex, connected_components_list, component_index);
        }
    }

    return connected_components_list;
}

/* Get strongly connected components using Kosaraju’s algorithm */
std::vector<std::vector<int>> DirectedGraph::StronglyConnectedComponents()
    const {
    std::vector<std::vector<int>> connected_components_list;

    std::vector<bool> visited_vertices(num_vertices_, false);
    std::deque<int> finished_vertices_stack;

    for (int vertex = 0; vertex < num_vertices_; ++vertex) {
        if (!visited_vertices[vertex]) {
            DFSForSCC(visited_vertices, finished_vertices_stack, vertex);
        }
    }

    for (int vertex = 0; vertex < num_vertices_; ++vertex) {
        visited_vertices[vertex] = false;
    }

    for (auto itr = finished_vertices_stack.rbegin();
         itr != finished_vertices_stack.rend(); ++itr) {
        int vertex = *itr;
        if (!visited_vertices[vertex]) {
            connected_components_list.push_back(std::vector<int>());
            ReverseDFSForSCC(visited_vertices, connected_components_list.back(),
                             vertex);
        }
    }

    return connected_components_list;
}

std::vector<std::vector<int>> DirectedGraph::SimpleCycles() const {
    std::unordered_set<int> blocked_set;
    std::unordered_map<int, std::unordered_set<int>> blocked_map;
    std::deque<int> stack;
    std::vector<std::vector<int>> cycles;

    std::vector<std::vector<int>> strongly_connected_components =
        StronglyConnectedComponents();

    for (auto& component : strongly_connected_components) {
        if (component.size() == 1) {
            continue;
        }

        std::sort(component.begin(), component.end());
        for (int i = 0; i < component.size(); ++i) {
            blocked_set.clear();
            blocked_map.clear();
            stack.clear();
            DirectedGraph search_graph = CreateSubgraph(
                std::vector<int>(component.begin() + i, component.end()));
            FindCyclesInSCCJohnson(search_graph, blocked_set, blocked_map,
                                   stack, cycles, component[i], component[i]);
        }
    }

    return cycles;
}

DirectedGraph DirectedGraph::CreateSubgraph(
    const std::vector<int>& vertices) const {
    std::unordered_set<int> vertices_set(vertices.begin(), vertices.end());
    std::vector<std::pair<int, int>> result_edges;

    for (auto& vertex : vertices) {
        for (auto& adjacent_vertex : adjacency_list_[vertex]) {
            if (vertices_set.count(adjacent_vertex)) {
                result_edges.push_back({vertex, adjacent_vertex});
            }
        }
    }

    return DirectedGraph(result_edges, num_vertices_);
}

DirectedGraph DirectedGraph::DeleteCyclesOfLength2() const {
    std::vector<std::pair<int, int>> result_edges;

    for (int vertex = 0; vertex < num_vertices_; ++vertex) {
        for (auto& adjacent_vertex : adjacency_list_[vertex]) {
            if (std::find(adjacency_list_[adjacent_vertex].begin(),
                          adjacency_list_[adjacent_vertex].end(),
                          vertex) == adjacency_list_[adjacent_vertex].end()) {
                result_edges.push_back({vertex, adjacent_vertex});
            }
        }
    }

    return DirectedGraph(result_edges, num_vertices_);
}

void DirectedGraph::PathSearch(
    int vertex, std::vector<std::vector<int>>& connected_components_list,
    int component_index) const {
    connected_components_list[component_index].push_back(vertex);
    std::vector<int> current_component(
        connected_components_list[component_index]);

    bool first_time = true;
    for (auto& adjacent_vertex : adjacency_list_[vertex]) {
        if (std::find(current_component.begin(), current_component.end(),
                      adjacent_vertex) != current_component.end()) {
            throw std::runtime_error("directed graph must be a DAG.");
        }

        if (first_time) {
            PathSearch(adjacent_vertex, connected_components_list,
                       component_index);
            first_time = false;
        } else {
            connected_components_list.push_back(
                std::vector<int>(current_component));
            PathSearch(adjacent_vertex, connected_components_list,
                       connected_components_list.size() - 1);
        }
    }
}

void DirectedGraph::DFSForSCC(std::vector<bool>& is_visited,
                              std::deque<int>& finished_vertices,
                              int vertex) const {
    is_visited[vertex] = true;
    for (auto& adjacent_vertex : adjacency_list_[vertex]) {
        if (!is_visited[adjacent_vertex]) {
            DFSForSCC(is_visited, finished_vertices, adjacent_vertex);
        }
    }
    finished_vertices.push_back(vertex);
}

void DirectedGraph::ReverseDFSForSCC(std::vector<bool>& is_visited,
                                     std::vector<int>& component,
                                     int vertex) const {
    is_visited[vertex] = true;
    component.push_back(vertex);
    for (auto& radjacent_vertex : reverse_adjacency_list_[vertex]) {
        if (!is_visited[radjacent_vertex]) {
            ReverseDFSForSCC(is_visited, component, radjacent_vertex);
        }
    }
}

void DirectedGraph::UnBlockJohnson(
    std::unordered_set<int>& blocked_set,
    std::unordered_map<int, std::unordered_set<int>>& blocked_map,
    int vertex) const {
    blocked_set.erase(vertex);
    if (blocked_map.count(vertex) > 0) {
        for (auto& blocked_vertex : blocked_map[vertex]) {
            if (blocked_set.count(blocked_vertex) > 0) {
                UnBlockJohnson(blocked_set, blocked_map, blocked_vertex);
            }
        }
        blocked_map.erase(vertex);
    }
}

/* Find all simple cycles starting with start_vertex using Johnson's algorithm
 */
bool DirectedGraph::FindCyclesInSCCJohnson(
    const DirectedGraph& scc_graph, std::unordered_set<int>& blocked_set,
    std::unordered_map<int, std::unordered_set<int>>& blocked_map,
    std::deque<int>& stack, std::vector<std::vector<int>>& cycles,
    int start_vertex, int current_vertex) const {
    bool found_cycle = false;
    stack.push_back(current_vertex);
    blocked_set.insert(current_vertex);

    for (auto& adjacent_vertex : scc_graph.AdjacentVertices(current_vertex)) {
        if (adjacent_vertex < start_vertex) {
            continue;
        }

        if (adjacent_vertex == start_vertex) {
            stack.push_back(start_vertex);
            std::vector<int> cycle(stack.begin(), stack.end());
            cycles.push_back(cycle);
            stack.pop_back();
            found_cycle = true;
        } else if (blocked_set.count(adjacent_vertex) == 0) {
            bool got_cycle = FindCyclesInSCCJohnson(
                scc_graph, blocked_set, blocked_map, stack, cycles,
                start_vertex, adjacent_vertex);
            found_cycle = found_cycle || got_cycle;
        }
    }

    if (found_cycle) {
        UnBlockJohnson(blocked_set, blocked_map, current_vertex);
    } else {
        for (auto& adjacent_vertex :
             scc_graph.AdjacentVertices(current_vertex)) {
            if (adjacent_vertex < start_vertex) {
                continue;
            }

            if (blocked_map.count(adjacent_vertex) == 0) {
                blocked_map[adjacent_vertex] = std::unordered_set<int>();
            }
            blocked_map[adjacent_vertex].insert(current_vertex);
        }
    }
    stack.pop_back();
    return found_cycle;
}
}  // namespace FTMR