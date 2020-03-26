#ifndef PATHFINDINGHELPERS_H
#define PATHFINDINGHELPERS_H

//==============================================================================
// File Description:
//
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "pathfinding.h"

#include <vector>
#include <memory>
#include <unordered_map>

// Stores node details and how fast its gotten there from the start
struct aStarNode {
    aStarNode() : intID(0), parentInt(0), parentEdge(0), 
                  bestTime(std::numeric_limits<double>::max()) { }
    
    aStarNode(IntersectionIndex intId, IntersectionIndex pInt, 
              StreetSegmentIndex pEdge, double time) :
              intID(intId), parentInt(pInt), parentEdge(pEdge), bestTime(time) { }
    
    IntersectionIndex intID;
    IntersectionIndex parentInt;
    StreetSegmentIndex parentEdge;
    double bestTime;
};

// Stores wave front element including the node its at and how it got there
struct waveElem {
    waveElem(aStarNode* pNode, IntersectionIndex pInt, IntersectionIndex pEdge, 
            double time, double estTotal) : 
            node(pNode), parentInt(pInt), parentEdge(pEdge), timeToNode(time), estTotalTime(estTotal) { }
    
    aStarNode* node;
    IntersectionIndex parentInt;
    StreetSegmentIndex parentEdge;
    double timeToNode;
    double estTotalTime;
};

// Custom comparison function for our A* priority queue
struct compare {
    bool operator() (const waveElem& lhs, const waveElem& rhs) {
        return lhs.estTotalTime > rhs.estTotalTime;
    }
};

// Finds the aStarNode* of a given intersection ID
aStarNode* getToNode(const IntersectionIndex& intID, 
                     const std::unordered_map<unsigned, aStarNode*>& visited);

// Reconstructs the path taken to get to the end 
std::vector<StreetSegmentIndex> findPathTaken(const std::unordered_map<unsigned, aStarNode*> visited, 
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt,
                                              const bool& clearMap);

// Determines if there was a turn and returns how much it should cost if there was
double determineTurnPenalty(const StreetSegmentIndex& fromEdge, 
                            const StreetSegmentIndex& toEdge, 
                            const double& turnPenalty);


#endif /* PATHFINDINGHELPERS_H */

