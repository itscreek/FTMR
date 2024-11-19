#include <iostream>
#include <vector>

#include "multitree_recolorability.hpp"

int main() {
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5},
        {5, 6}, {7, 2}, {2, 8}, {3, 9}, {10, 4}};
    FTMR::MultitreeRecolorability recolorability(edges, 11);

    bool condition_s = recolorability.CheckConditionS();
    bool condition_cp = recolorability.CheckConditionCP();
    bool condition_cv = recolorability.CheckConditionCV();

    std::cout << "Condition S: " << (condition_s ? "True" : "False")
              << std::endl;
    std::cout << "Condition CP: " << (condition_cp ? "True" : "False")
              << std::endl;
    std::cout << "Condition CV: " << (condition_cv ? "True" : "False")
              << std::endl;

    recolorability.OutputInfoOfPathRelationGraph(
        "polytree_/not_ncc2/v11_2_path.dot", false);
}