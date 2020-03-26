//==============================================================================
// File Description:
//
//==============================================================================

#include "pathfindingHelpers.h"

#include <algorithm>

//
std::vector<StreetSegmentIndex> findPathTaken(const std::vector<aStarNode>& cameFrom,
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt) {
    int current = endInt;
    std::vector<int> result;
    while (current != startInt) {
        result.push_back(cameFrom[current].parentEdge);
        current = cameFrom[current].parentInt;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

//
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