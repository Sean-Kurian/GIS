#ifndef PATHFINDINGHELPERS_H
#define PATHFINDINGHELPERS_H

//==============================================================================
// File Description:
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "pathfinding.h"

#include <vector>

struct aStarNode {
    aStarNode() : intID(0), parentInt(0), parentEdge(0), 
                  timeToNode(std::numeric_limits<double>::max()), estTotalTime(0) { }
    
    aStarNode(IntersectionIndex intInd, IntersectionIndex pInt, StreetSegmentIndex pEdge, 
              double timeToN, double estT) :
              intID(intInd), parentInt(pInt), parentEdge(pEdge), timeToNode(timeToN), estTotalTime(estT) { }
    
    IntersectionIndex intID;
    IntersectionIndex parentInt;
    StreetSegmentIndex parentEdge;
    double timeToNode; 
    double estTotalTime;
};

struct compare {
    bool operator() (const aStarNode& lhs, const aStarNode& rhs) {
        return lhs.estTotalTime > rhs.estTotalTime;
    }
};


//
std::vector<StreetSegmentIndex> findPathTaken(const std::vector<aStarNode>& cameFrom, 
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt);

//
double determineTurnPenalty(const StreetSegmentIndex& fromEdge, 
                            const StreetSegmentIndex& toEdge, 
                            const double& turnPenalty);


#endif /* PATHFINDINGHELPERS_H */

