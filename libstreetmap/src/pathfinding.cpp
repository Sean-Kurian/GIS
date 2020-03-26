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
#include <unordered_map>

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
    // Nodes to explore next
    std::priority_queue<waveElem, std::vector<waveElem>, compare> openSet;
    //
    std::unordered_map<unsigned, aStarNode*> visited;
    
    aStarNode baseNode(intersect_id_start, -1, -1, std::numeric_limits<double>::max());
    visited.insert(std::make_pair(intersect_id_start, &baseNode));
    
    double distToEnd = find_distance_between_two_points(std::make_pair(
                            getIntersectionPosition(intersect_id_start),
                            getIntersectionPosition(intersect_id_end)));
    // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
    double estTotalTime = distToEnd * 0.01 * 3.6;
    
    waveElem baseElem(&baseNode, -1, -1, 0, estTotalTime); 
    openSet.push(baseElem);
    while (!openSet.empty()) {
        waveElem wave = openSet.top();  // Get next element
        openSet.pop();                  // Remove from wavefront
        aStarNode* currNode = wave.node;
        if (currNode->intID == intersect_id_end) 
           return findPathTaken(visited, intersect_id_start, intersect_id_end);
        
        if (wave.timeToNode < currNode->bestTime) {
            currNode->bestTime = wave.timeToNode;
            currNode->parentInt = wave.parentInt;
            currNode->parentEdge = wave.parentEdge;
            
            std::vector<pairSegIntID> adjSegIntIDs = gData.getAdjacentSegIntIDsOfInt(currNode->intID);
            for (const pairSegIntID& segIntID : adjSegIntIDs) {
                
                aStarNode* toNode = getToNode(segIntID.second, visited);
                if (toNode == NULL) {
                    toNode = new aStarNode(segIntID.second, currNode->intID, 
                                           segIntID.first, 
                                           std::numeric_limits<double>::max());
                    visited.insert(std::make_pair(segIntID.second, toNode));
                }

                double timeToNode = wave.timeToNode + gData.getTravelTimeOfSeg(segIntID.first)
                                  + determineTurnPenalty(wave.parentEdge, segIntID.first, turn_penalty);
                distToEnd = find_distance_between_two_points(std::make_pair(
                                        getIntersectionPosition(segIntID.second),
                                        getIntersectionPosition(intersect_id_end)));
                // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
                estTotalTime = timeToNode + (distToEnd * 0.01 * 3.6);
                // Add neighbour to open set
                openSet.push(waveElem(toNode, currNode->intID, segIntID.first, timeToNode, estTotalTime));
            }
        }
    }
//    std::vector<aStarNodex> cameFrom;
//    cameFrom.resize(getNumIntersections());
//    
//    std::priority_queue<aStarNodex, std::vector<aStarNodex>, compare> openSet;
//    aStarNodex baseNode(intersect_id_start, -1, -1, 0, 0);
//    openSet.push(baseNode);
//    cameFrom[intersect_id_start].intID = intersect_id_start;
//    
//    while (!openSet.empty()) {
//        aStarNodex currNode = openSet.top();
//        
//        if (currNode.intID == intersect_id_end)
//            return findPathTaken(cameFrom, intersect_id_start, intersect_id_end);
//
//        openSet.pop();
//        std::vector<pairSegIntID> adjSegIntIDs = gData.getAdjacentSegIntIDsOfInt(currNode.intID);
//        std::vector<int> segsOfInt = gData.getSegsOfIntersection(currNode.intID);
//        
//        for (const pairSegIntID& segIntID : adjSegIntIDs) {
//            InfoStreetSegment SSData = getInfoStreetSegment(segIntID.first);
//
//            if (segIntID.second == intersect_id_end) {
//                cameFrom[segIntID.second].parentEdge = segIntID.first;
//                cameFrom[segIntID.second].parentInt = currNode.intID;
//                return findPathTaken(cameFrom, intersect_id_start, intersect_id_end);
//            }
//            
//            double segTravelTime = gData.getTravelTimeOfSeg(segIntID.first);
//            double turnPenalty = determineTurnPenalty(currNode.parentEdge, segIntID.first, turn_penalty);
//            double timeToInt = segTravelTime + currNode.timeToNode + turnPenalty;
//            
//            // New fastest way to this node
//            if (timeToInt < cameFrom[segIntID.second].timeToNode) {
//                cameFrom[segIntID.second].intID = segIntID.second;
//                cameFrom[segIntID.second].parentInt = currNode.intID;
//                cameFrom[segIntID.second].parentEdge = segIntID.first;
//                cameFrom[segIntID.second].timeToNode = timeToInt;
//                double distToEnd = find_distance_between_two_points(std::make_pair(
//                                        getIntersectionPosition(segIntID.second),
//                                        getIntersectionPosition(intersect_id_end)));
//                // Est time to end is distance * 1 / 100 (1 / speed limit) * 3.6 (conversion)
//                cameFrom[segIntID.second].estTotalTime = timeToInt + (distToEnd * 0.01 * 3.6);
//                // Add neighbour to open set
//                openSet.push(cameFrom[segIntID.second]);
//            }
//        }
//    }
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
