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

//struct aStarNodex {
//    aStarNodex() : intID(0), parentInt(0), parentEdge(0), 
//                  timeToNode(std::numeric_limits<double>::max()), estTotalTime(0) { }
//    
//    aStarNodex(IntersectionIndex intInd, IntersectionIndex pInt, StreetSegmentIndex pEdge, 
//              double timeToN, double estT) :
//              intID(intInd), parentInt(pInt), parentEdge(pEdge), timeToNode(timeToN), estTotalTime(estT) { }
//    
//    IntersectionIndex intID;
//    IntersectionIndex parentInt;
//    StreetSegmentIndex parentEdge;
//    double timeToNode; 
//    double estTotalTime;
//};

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

struct compare {
    bool operator() (const waveElem& lhs, const waveElem& rhs) {
        return lhs.estTotalTime > rhs.estTotalTime;
    }
};

//
aStarNode* getToNode(const IntersectionIndex& intID, 
                     const std::unordered_map<unsigned, aStarNode*>& visited);

std::vector<StreetSegmentIndex> findPathTaken(const std::unordered_map<unsigned, aStarNode*> visited, 
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt);

////
//std::vector<StreetSegmentIndex> findPathTakenx(const std::vector<aStarNodex>& cameFrom, 
//                                              const IntersectionIndex& startInt,
//                                              const IntersectionIndex& endInt);

//
double determineTurnPenalty(const StreetSegmentIndex& fromEdge, 
                            const StreetSegmentIndex& toEdge, 
                            const double& turnPenalty);


#endif /* PATHFINDINGHELPERS_H */

