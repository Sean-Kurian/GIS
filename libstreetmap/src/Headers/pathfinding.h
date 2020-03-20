//==============================================================================
// File Description:
//
//==============================================================================


#ifndef PATHFINDING_H
#define PATHFINDING_H
#include  "StreetsDatabaseAPI.h"
#include <vector>
#include <string> 

double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, const double turn_penalty); 

std::vector<StreetSegmentIndex> find_path_between_intersections(const IntersectionIndex intersection_id_start, 
            const IntersectionIndex intersect_id_end, const double turn_penalty);   

double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
        const double walking_speed, const double turn_penalty); 

std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> find_path_with_walk_to_pickup(
        const IntersectionIndex start_intersection, 
        const IntersectionIndex end_intersection, 
        const double turn_penalty, 
        const double walking_speed, 
        const double walking_time_limit); 


#endif /* PATHFINDING_H */

