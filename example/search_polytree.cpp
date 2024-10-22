#include <filesystem>
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

std::string& ProjectDirName() {
    static std::string dir_name;
    return dir_name;
}

constexpr char kTreesDataDir[] = "example/trees/";
constexpr char kTreesFileName[] = "trees_data";

enum class TypeOfPolytree {
    kS,
    kCPNotS,
    kCVNotCP,
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
        return TypeOfPolytree::kCVNotCP;
    } else {
        return TypeOfPolytree::kNotTractable;
    }
}

std::vector<EdgesList> GetTrees(int num_vertices) {
    std::vector<EdgesList> trees_list;
    std::string filename = ProjectDirName() + std::string(kTreesDataDir) +
                           std::string(kTreesFileName) + "_" +
                           std::to_string(num_vertices) + ".txt";

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Trees file not found." << std::endl;
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

void SaveGraphsDOT(const EdgesList& edges_list,
                   std::filesystem::path file_path) {
    std::ofstream file(file_path);

    file << "digraph {" << std::endl;
    for (auto& edge : edges_list) {
        file << edge.first << "->" << edge.second << ";" << std::endl;
    }
    file << "}" << std::endl;
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

void SearchAllPolytrees(int num_vertices, std::string save_dir_name,
                        bool save_S, bool save_CP_not_S, bool save_CV_not_CP,
                        bool save_others) {
    std::cout << "Searching all polytrees with " << num_vertices << " vertices."
              << std::endl;
    std::cout << "Running..." << std::endl;

    std::filesystem::path save_dir_path(save_dir_name);
    if (save_S || save_CP_not_S || save_CV_not_CP || save_others) {
        std::filesystem::create_directory(save_dir_path);
    }
    std::filesystem::path S_dir_path = save_dir_path;
    S_dir_path.append("satisfying_S");
    std::filesystem::path CP_dir_path = save_dir_path;
    CP_dir_path.append("satisfying_CP_not_S");
    std::filesystem::path CV_dir_path = save_dir_path;
    CV_dir_path.append("satisfying_CV_not_CP");
    std::filesystem::path others_dir_path = save_dir_path;
    others_dir_path.append("others");

    if (save_S) {
        std::filesystem::create_directory(S_dir_path);
    }

    if (save_CP_not_S) {
        std::filesystem::create_directory(CP_dir_path);
    }

    if (save_CV_not_CP) {
        std::filesystem::create_directory(CV_dir_path);
    }

    if (save_others) {
        std::filesystem::create_directory(others_dir_path);
    }

    std::vector<EdgesList> trees_list = GetTrees(num_vertices);

    if (trees_list.size() == 0) {
        return;
    }

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
                    if (save_S) {
                        std::filesystem::path file_path = S_dir_path;
                        file_path.append("S_" + std::to_string(num_s) + ".dot");
                        SaveGraphsDOT(new_edges_list, file_path);
                    }
                    ++num_s;
                    break;
                case TypeOfPolytree::kCPNotS:
                    if (save_CP_not_S) {
                        std::filesystem::path file_path = CP_dir_path;
                        file_path.append("CP_" + std::to_string(num_cp_not_s) +
                                         ".dot");
                        SaveGraphsDOT(new_edges_list, file_path);
                    }
                    ++num_cp_not_s;
                    break;
                case TypeOfPolytree::kCVNotCP:
                    if (save_CV_not_CP) {
                        std::filesystem::path file_path = CV_dir_path;
                        file_path.append("CV_" + std::to_string(num_cv) +
                                         ".dot");
                        SaveGraphsDOT(new_edges_list, file_path);
                    }
                    ++num_cv;
                    break;
                case TypeOfPolytree::kNotTractable:
                    if (save_others) {
                        std::filesystem::path file_path = others_dir_path;
                        file_path.append("others_" +
                                         std::to_string(num_not_tractable) +
                                         ".dot");
                        SaveGraphsDOT(new_edges_list, file_path);
                    }
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
    std::cout << "    Satisfying (CV) not (CP): " << num_cv << std::endl;
    std::cout << "    Others: " << num_not_tractable << std::endl;
}
}  // namespace FTMRSearch

int main(int argc, char* argv[]) {
    std::string& project_dir_name = FTMRSearch::ProjectDirName();
    project_dir_name = argv[1];
    project_dir_name += "/";

    int num_vertices = -1;
    bool save_S = false;
    bool save_CP_not_S = false;
    bool save_CV_not_CP = false;
    bool save_others = false;
    std::string save_dir_name = "";
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--save") == 0) {
            if (i + 1 == argc || argv[i + 1][0] == '-') {
                save_S = true;
                save_CP_not_S = true;
                save_CV_not_CP = true;
                save_others = true;
                break;
            }

            for (int j = 0; argv[i + 1][j] != '\0'; ++j) {
                char c = argv[i + 1][j];
                if (c == 'S') {
                    save_S = true;
                } else if (c == 'P') {
                    save_CP_not_S = true;
                } else if (c == 'V') {
                    save_CV_not_CP = true;
                } else if (c == 'O') {
                    save_others = true;
                } else {
                    std::cout
                        << "Invalid arguments of --save option: " << argv[i + 1]
                        << std::endl;
                    std::cout << "usage: --save [S][P][V][O]" << std::endl;
                }
            }
            ++i;
            continue;
        }

        if (strcmp(argv[i], "--save-dir") == 0) {
            if (i + 1 == argc || argv[i + 1][0] == '-') {
                std::cout << "usage: --save-dir dir" << std::endl;
                break;
            }
            save_dir_name = argv[i + 1];
            ++i;
            continue;
        }

        if (argv[i][0] == '-') {
            std::cout << "invalid option " << argv[i] << std::endl;
            std::cout << "usage: <num_vertices> [--save types] [--save-dir dir]"
                      << std::endl;
        }

        try {
            num_vertices = std::stoi(argv[i]);
        } catch (const std::invalid_argument& e) {
            std::cout << "invalid arguments: " << argv[i] << std::endl;
            std::cout << "usage: <num_vertices> [--save types] [--save-dir dir]"
                      << std::endl;
        }
    }

    if (save_dir_name == "") {
        save_dir_name = project_dir_name + "/example/polytree_" +
                        std::to_string(num_vertices);
    }

    FTMRSearch::SearchAllPolytrees(num_vertices, save_dir_name, save_S,
                                   save_CP_not_S, save_CV_not_CP, save_others);
}