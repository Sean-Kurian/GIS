//==============================================================================
// File Description: Defines functions used to compute paths between 2 points
// and the time needed to travel those paths walking or with a car
//==============================================================================

#ifndef LIBCURL_H


#include "pathfinding.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "globalData.h"
#include "math.h"

double compute_path_travel_time(const std::vector<StreetSegmentIndex>& path, 
        const double turn_penalty){
    
        if (path.size() == 0) return 0; 
        
        double travel_time = 0; 
            InfoStreetSegment SSData, SSData2; 
            for (int i = 0; i < path.size()-1; i++){
                SSData = getInfoStreetSegment(path[i]); 
                SSData2 = getInfoStreetSegment(path[i+1]); 

                travel_time += find_street_segment_travel_time(path[i]); 
                if(SSData.streetID != SSData2.streetID){
                    travel_time += turn_penalty; 
                } 
            }
            travel_time += find_street_segment_travel_time(path[path.size() - 1]); 
            return travel_time; 
}
//length over walking speed
double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
        const double walking_speed, 
        const double turn_penalty){
    
            if (path.size() == 0) return 0; 
    
            double travel_time = 0; 
            InfoStreetSegment SSData, SSData2; 
            for (int i = 0; i < path.size()-1; i++){
                SSData = getInfoStreetSegment(path[i]); 
                SSData2 = getInfoStreetSegment(path[i+1]); 

                travel_time += find_street_segment_length(path[i]) / walking_speed; 
                if(SSData.streetID != SSData2.streetID){
                    travel_time += turn_penalty; 
                } 
            }
            travel_time += find_street_segment_length(path[path.size() - 1]) / 1; 
            return travel_time; 
}

double compute_path_walking_time(const std::vector<StreetSegmentIndex>& path, 
        const double walking_speed, const double turn_penalty); 

std::vector<StreetSegmentIndex> find_path_between_intersections(
        const IntersectionIndex intersect_id_start,
            const IntersectionIndex intersect_id_end, 
            const double turn_penalty){
    
}

std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> find_path_with_walk_to_pickup(
        const IntersectionIndex start_intersection, 
        const IntersectionIndex end_intersection, 
        const double turn_penalty, 
        const double walking_speed, 
        const double walking_time_limit){
    
}
