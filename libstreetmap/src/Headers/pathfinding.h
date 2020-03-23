#ifndef PATHFINDING_H
#define PATHFINDING_H

//==============================================================================
// File Description: Defines functions used to compute paths between 2 points
// and the time needed to travel those paths walking or with a car
//==============================================================================

#include "StreetsDatabaseAPI.h"

#include <vector>
#include <string> 
#include <limits>
#include <iostream>

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

//
std::vector<StreetSegmentIndex> findPathTaken(const std::vector<aStarNode>& cameFrom, 
                                              const IntersectionIndex& startInt,
                                              const IntersectionIndex& endInt);

//
double determineTurnPenalty(const StreetSegmentIndex& fromEdge, const StreetSegmentIndex& toEdge, 
                            const double& turnPenalty);

//
double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, const double turn_penalty); 

//
std::vector<StreetSegmentIndex> find_path_between_intersections(const IntersectionIndex intersection_id_start, 
            const IntersectionIndex intersect_id_end, const double turn_penalty);   

//
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
        const double walking_speed, const double turn_penalty); 

//
std::vector<StreetSegmentIndex> find_path_between_intersections(
        const IntersectionIndex intersect_id_start,
            const IntersectionIndex intersect_id_end, 
            const double turn_penalty); 

//
std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> find_path_with_walk_to_pickup(
        const IntersectionIndex start_intersection, 
        const IntersectionIndex end_intersection, 
        const double turn_penalty, 
        const double walking_speed, 
        const double walking_time_limit); 


#endif /* PATHFINDING_H */

