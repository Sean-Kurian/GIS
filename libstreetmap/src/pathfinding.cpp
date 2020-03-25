//==============================================================================
// File Description: Defines functions used to compute paths between 2 points
// and the time needed to travel those paths walking or with a car
//==============================================================================

#include "pathfinding.h"
#include "pathfindingHelpers.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "globalData.h"
#include "math.h"

#include <queue>

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
    for (int i = 0; i < path.size() - 1; i++) {
        SSData = getInfoStreetSegment(path[i]); 
        SSData2 = getInfoStreetSegment(path[i + 1]); 

        travel_time += find_street_segment_length(path[i]) / walking_speed; 
        if(SSData.streetID != SSData2.streetID){
            travel_time += turn_penalty; 
        } 
    }
    travel_time += find_street_segment_length(path[path.size() - 1]) / 1; 
    return travel_time; 
}

std::vector<StreetSegmentIndex> find_path_between_intersections(
                                const IntersectionIndex intersect_id_start,
                                const IntersectionIndex intersect_id_end, 
                                const double turn_penalty) {
    std::vector<int> segsOfInt;
    std::vector<aStarNode> cameFrom;
    cameFrom.resize(getNumIntersections());
    
    std::priority_queue<aStarNode, std::vector<aStarNode>, compare> openSet;
    double baseDist = find_distance_between_two_points(std::make_pair(
                                    getIntersectionPosition(intersect_id_start),
                                    getIntersectionPosition(intersect_id_end)));
    // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
    double baseEstTime = baseDist * 0.01 * 3.6;
    aStarNode baseNode(intersect_id_start, -1, -1, 0, baseEstTime);
    openSet.push(baseNode);
    cameFrom[intersect_id_start].intID = intersect_id_start;
    
    while (!openSet.empty()) {
        aStarNode currNode = openSet.top();
        
//        std::cout << "Checking intersection: " << currNode.intID << "\n";
        
        if (currNode.intID == intersect_id_end) {
            // FIND PATH TAKEN
            return findPathTaken(cameFrom, intersect_id_start, intersect_id_end);
        }
        openSet.pop();
        segsOfInt = gData.getSegsOfIntersection(currNode.intID);
        
        for (const int& segID : segsOfInt) {
            InfoStreetSegment SSData = getInfoStreetSegment(segID);
            int toInt;
            
            if (SSData.from == currNode.intID) {
                toInt = SSData.to;
                if (toInt == intersect_id_end) {
//                    std::cout << "Path Found\n";
                    cameFrom[toInt].parentEdge = segID;
                    cameFrom[toInt].parentInt = currNode.intID;
                    return findPathTaken(cameFrom, intersect_id_start, intersect_id_end);
                }
            }
            else {
                if (SSData.oneWay)
                    continue;
                else
                    toInt = SSData.from;
            }
            
            double segTravelTime = gData.getTravelTimeOfSegment(segID);
            double turnPenalty = determineTurnPenalty(currNode.parentEdge, segID, turn_penalty);
            double timeToInt = segTravelTime + currNode.timeToNode + turnPenalty;
            
            // New fastest way to this node
            if (timeToInt < cameFrom[toInt].timeToNode) {
                cameFrom[toInt].intID = toInt;
                cameFrom[toInt].parentInt = currNode.intID;
                cameFrom[toInt].parentEdge = segID;
                cameFrom[toInt].timeToNode = timeToInt;
                double distToEnd = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(toInt),
                                        getIntersectionPosition(intersect_id_end)));
                // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
                cameFrom[toInt].estTotalTime = timeToInt + (distToEnd * 0.01 * 3.6);
                // Add neighbour to open set
                openSet.push(cameFrom[toInt]);
            }
        }
    }
    // Open set empty but no path found
    std::cerr << "Open set empty but no path found. May be impossible\n"; 
    std::vector<int> failure = {};
    return failure;
}

std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> find_path_with_walk_to_pickup(
                                const IntersectionIndex start_intersection, 
                                const IntersectionIndex end_intersection, 
                                const double turn_penalty, 
                                const double walking_speed, 
                                const double walking_time_limit) {
    
}
