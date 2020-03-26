#ifndef PATHFINDING_H
#define PATHFINDING_H

//==============================================================================
// File Description: Defines functions used to compute paths between 2 points
// and the time needed to travel those paths walking or with a car
//==============================================================================

#include "StreetsDatabaseAPI.h"
#include "pathfindingHelpers.h"

#include <vector>
#include <string> 
#include <limits>
#include <iostream>

// Uses A* algorithm to find fastest path between 2 intersections
double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, const double turn_penalty); 

// Computes the time needed to walk a given path at a given speed
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
                                 const double walking_speed, 
                                 const double turn_penalty); 

// Uses A* algorithm to find fastest path between 2 intersections
std::vector<StreetSegmentIndex> find_path_between_intersections(
                                 const IntersectionIndex intersect_id_start,
                                 const IntersectionIndex intersect_id_end, 
                                 const double turn_penalty); 

// Uses A* to find fastest path between 2 intersections with walking to a start point
std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex> > find_path_with_walk_to_pickup(
                                 const IntersectionIndex start_intersection, 
                                 const IntersectionIndex end_intersection, 
                                 const double turn_penalty, 
                                 const double walking_speed, 
                                 const double walking_time_limit); 

#endif /* PATHFINDING_H */

