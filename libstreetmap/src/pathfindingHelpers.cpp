//==============================================================================
// File Description: Calculations to be used in pathfinding
//
//==============================================================================

#include "pathfindingHelpers.h"

#include <algorithm>

// Finds the aStarNode* of a given intersection ID
aStarNode* getToNode(const IntersectionIndex& intID, 
                     const std::unordered_map<unsigned, aStarNode*>& visited) {
    auto itr = visited.find(intID);
    if (itr == visited.end())
        return NULL;
    else
        return itr->second;
}

// Reconstructs the path taken to get to the end
std::vector<StreetSegmentIndex> findPathTaken(const std::unordered_map<unsigned, aStarNode*> visited, 
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt,
                                              const bool& clearMap) {
    unsigned current = endInt;
    std::vector<int> result;
    while (current != startInt) {
        auto itr = visited.find(current);
        result.push_back(itr->second->parentEdge);
        current = itr->second->parentInt;
    }
    std::reverse(result.begin(), result.end());
    if (clearMap)
        for (auto& mapElem : visited)
            delete mapElem.second;
    return result;
}

// Determines if there was a turn and returns how much it should cost if there was
double determineTurnPenalty(const StreetSegmentIndex& fromEdge,
                            const StreetSegmentIndex& toEdge,
                            const double& turnPenalty) {
    if (fromEdge != -1) {
        InfoStreetSegment fromData = getInfoStreetSegment(fromEdge);
        InfoStreetSegment toData = getInfoStreetSegment(toEdge);
        if (fromData.streetID == toData.streetID)
            return 0;
        else
            return turnPenalty;
    }
    else
        return 0;
}
