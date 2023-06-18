#include "directed_graph.hpp"

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

int DirectedGraph::OutDegree(int vertex) {
    if (InvalidVertexNumber(vertex)) {
        throw std::invalid_argument("Vertex number must be 0 to n - 1.");
    }

    return adjacency_list_[vertex].size();
}

int DirectedGraph::InDegree(int vertex) {
    if (InvalidVertexNumber(vertex)) {
        throw std::invalid_argument("Vertex number must be 0 to n - 1.");
    }

    return reverse_adjacency_list_[vertex].size();
}

std::vector<std::vector<int>> DirectedGraph::UnilaterallyConnectedComponents() {
    std::vector<std::vector<int>> connected_components_list;

    for (int vertex = 0; vertex < num_vertices_; vertex++) {
        if (reverse_adjacency_list_[vertex].size() == 0)
        {
            connected_components_list.push_back(std::vector<int>());
            int component_index = connected_components_list.size() - 1;
            PathSearch(vertex, connected_components_list, component_index);
        }
    }

    return connected_components_list;
}

void DirectedGraph::PathSearch(int vertex, std::vector<std::vector<int>>& connected_components_list, int component_index){
    connected_components_list[component_index].push_back(vertex);
    std::vector<int> current_component(connected_components_list[component_index]);

    for (int i = 0; i < adjacency_list_[vertex].size(); i++){
        if (i == 0){
            PathSearch(adjacency_list_[vertex][i], connected_components_list, component_index);
        }
        else {
            connected_components_list.push_back(std::vector<int>(current_component));
            PathSearch(adjacency_list_[vertex][i], connected_components_list, connected_components_list.size() - 1);
        }
    }
}
}  // namespace FTMR