#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "multitree_recolorability.hpp"

namespace FTMRSearch {
using EdgesList = std::vector<std::pair<int, int>>;

constexpr char kTreesDataDir[] = "example/trees/";
constexpr char kTreesFileName[] = "trees_data";

enum class TypeOfPolytree {
    kS,
    kCPNotS,
    kCV,
    kNotTractable,
};

TypeOfPolytree ClassifyMultitree(const std::vector<std::pair<int, int>>& edges,
                                 int num_vertices) {
    FTMR::MultitreeRecolorability multitree(edges, num_vertices);
    if (multitree.CheckConditionS()) {
        return TypeOfPolytree::kS;
    } else if (multitree.CheckConditionCP()) {
        return TypeOfPolytree::kCPNotS;
    } else if (multitree.CheckConditionCV()) {
        return TypeOfPolytree::kCV;
    } else {
        return TypeOfPolytree::kNotTractable;
    }
}

std::vector<EdgesList> GetTrees(int num_vertices) {
    std::vector<EdgesList> trees_list;
    std::string filename = std::string(kTreesDataDir) +
                           std::string(kTreesFileName) + "_" +
                           std::to_string(num_vertices) + ".txt";

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Trees file not found" << std::endl;
        return trees_list;
    }
    std::string line;

    while (std::getline(file, line)) {
        if (line.substr(0, 5) != "Graph") {
            continue;
        }

        std::getline(file, line);
        std::string number;
        std::stringstream ss(line);
        ss >> number;
        int input_vertices = std::stoi(number);
        ss >> number;
        int input_edges = std::stoi(number);
        if (input_vertices != num_vertices || input_edges != num_vertices - 1) {
            throw std::runtime_error("Invalid trees data.");
        }

        EdgesList edges(num_vertices - 1);
        std::getline(file, line);
        ss = std::stringstream(line);
        for (int i = 0; i < num_vertices - 1; ++i) {
            ss >> number;
            int start_vertex = std::stoi(number);
            ss >> number;
            int end_vertex = std::stoi(number);
            edges[i] = {start_vertex, end_vertex};
        }

        trees_list.push_back(edges);
    }
    return trees_list;
}

EdgesList FlipEdges(const EdgesList& original, int flip_bits) {
    EdgesList result(original.size());
    for (int i = 0; i < original.size(); ++i) {
        if (flip_bits & (1 << i)) {
            result[i] = {original[i].second, original[i].first};
        } else {
            result[i] = original[i];
        }
    }
    return result;
}

void SearchAllPolytrees(int num_vertices) {
    std::cout << "Searching all polytrees with " << num_vertices << " vertices."
              << std::endl;
    std::cout << "Running..." << std::endl;

    std::vector<EdgesList> trees_list = GetTrees(num_vertices);

    int num_s = 0;
    int num_cp_not_s = 0;
    int num_cv = 0;
    int num_not_tractable = 0;
    for (auto& edges_list : trees_list) {
        for (int flip_bits = 0; flip_bits < (1 << (num_vertices - 1));
             ++flip_bits) {
            EdgesList new_edges_list = FlipEdges(edges_list, flip_bits);
            TypeOfPolytree type =
                ClassifyMultitree(new_edges_list, num_vertices);

            switch (type) {
                case TypeOfPolytree::kS:
                    ++num_s;
                    break;
                case TypeOfPolytree::kCPNotS:
                    ++num_cp_not_s;
                    break;
                case TypeOfPolytree::kCV:
                    ++num_cv;
                    break;
                case TypeOfPolytree::kNotTractable:
                    ++num_not_tractable;
                    break;
            }
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "Result: " << std::endl;
    std::cout << "Search " << (trees_list.size() * (1 << (num_vertices - 1)))
              << " polytrees." << std::endl;
    std::cout << "    Satisfying (S): " << num_s << std::endl;
    std::cout << "    Satisfying (CP) not (S): " << num_cp_not_s << std::endl;
    std::cout << "    Satisfying (CV): " << num_cv << std::endl;
    std::cout << "    Others: " << num_not_tractable << std::endl;
}
}  // namespace FTMRSearch

int main(int argc, char* argv[]) {
    if (argc == 1 || argc > 2) {
        std::cout << "Invalid arguments." << std::endl;
        return 0;
    }

    int num_vertices = std::stoi(argv[1]);
    FTMRSearch::SearchAllPolytrees(num_vertices);
}