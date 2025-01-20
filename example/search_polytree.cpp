#include <cstring>
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
                        bool save_cp, bool save_cv, bool save_nce,
                        bool save_others) {
    std::cout << "Searching all polytrees with " << num_vertices << " vertices."
              << std::endl;
    std::cout << "Running..." << std::endl;

    std::filesystem::path save_dir_path(save_dir_name);
    if (save_cp || save_cv || save_nce || save_others) {
        std::filesystem::create_directory(save_dir_path);
    }

    std::filesystem::path cp_dir_path = save_dir_path;
    cp_dir_path.append("cp");
    std::filesystem::path cv_dir_path = save_dir_path;
    cv_dir_path.append("cv");
    std::filesystem::path nce_dir_path = save_dir_path;
    nce_dir_path.append("nce");
    std::filesystem::path others_dir_path = save_dir_path;
    others_dir_path.append("others");

    if (save_cp) {
        std::filesystem::create_directory(cp_dir_path);
    }

    if (save_cv) {
        std::filesystem::create_directory(cv_dir_path);
    }

    if (save_nce) {
        std::filesystem::create_directory(nce_dir_path);
    }

    if (save_others) {
        std::filesystem::create_directory(others_dir_path);
    }

    std::vector<EdgesList> trees_list = GetTrees(num_vertices);

    if (trees_list.size() == 0) {
        return;
    }

    int num_cp = 0;
    int num_cv = 0;
    int num_nce = 0;
    int num_not_tractable = 0;
    for (auto& edges_list : trees_list) {
        for (int flip_bits = 0; flip_bits < (1 << (num_vertices - 1));
             ++flip_bits) {
            EdgesList new_edges_list = FlipEdges(edges_list, flip_bits);
            FTMR::MultitreeRecolorability multitree(new_edges_list,
                                                    num_vertices);

            bool isCP = multitree.CheckConditionCP();
            bool isCV = multitree.CheckConditionCV();
            bool isNCE = multitree.CheckConditionNCE();

            if (isCP) {
                ++num_cp;
                if (save_cp) {
                    std::filesystem::path file_path = cp_dir_path;
                    file_path.append("cp_" + std::to_string(num_cp) + ".dot");
                    SaveGraphsDOT(new_edges_list, file_path);
                }
            }

            if (isCV) {
                ++num_cv;
                if (save_cv) {
                    std::filesystem::path file_path = cv_dir_path;
                    file_path.append("cv_" + std::to_string(num_cv) + ".dot");
                    SaveGraphsDOT(new_edges_list, file_path);
                }
            }

            if (isNCE) {
                ++num_nce;
                if (save_nce) {
                    std::filesystem::path file_path = nce_dir_path;
                    file_path.append("nce_" + std::to_string(num_nce) + ".dot");
                    SaveGraphsDOT(new_edges_list, file_path);
                }
            }

            if (!isCP && !isCV && !isNCE) {
                ++num_not_tractable;
                if (save_others) {
                    std::filesystem::path file_path = others_dir_path;
                    file_path.append(
                        "others_" + std::to_string(num_not_tractable) + ".dot");
                    SaveGraphsDOT(new_edges_list, file_path);
                }
            }
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "Result: " << std::endl;
    std::cout << "Search " << (trees_list.size() * (1 << (num_vertices - 1)))
              << " polytrees." << std::endl;
    std::cout << "    (CP)        : " << num_cp << std::endl;
    std::cout << "    (CV)        : " << num_cv << std::endl;
    std::cout << "    (NCE)       : " << num_nce << std::endl;
    std::cout << "    None of them: " << num_not_tractable << std::endl;
}
}  // namespace FTMRSearch

int main(int argc, char* argv[]) {
    std::string& project_dir_name = FTMRSearch::ProjectDirName();
    project_dir_name = argv[1];
    project_dir_name += "/";

    int num_vertices = -1;
    bool save_cp = false;
    bool save_cv = false;
    bool save_nce = false;
    bool save_others = false;
    std::string save_dir_name = "";
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--save") == 0) {
            if (i + 1 == argc || argv[i + 1][0] == '-') {
                save_others = true;
                break;
            }

            for (int j = 0; argv[i + 1][j] != '\0'; ++j) {
                char c = argv[i + 1][j];
                if (c == 'P') {
                    save_cp = true;
                } else if (c == 'V') {
                    save_cv = true;
                } else if (c == 'N') {
                    save_nce = true;
                } else if (c == 'O') {
                    save_others = true;
                } else {
                    std::cout
                        << "Invalid arguments of --save option: " << argv[i + 1]
                        << std::endl;
                    std::cout << "usage: --save [S][C][O]" << std::endl;
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
        save_dir_name = project_dir_name + "/example/new_polytree_" +
                        std::to_string(num_vertices);
    }

    FTMRSearch::SearchAllPolytrees(num_vertices, save_dir_name, save_cp,
                                   save_cv, save_nce, save_others);
}