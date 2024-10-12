#include <utility>
#include <vector>

#include "multitree_recolorability.hpp"

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